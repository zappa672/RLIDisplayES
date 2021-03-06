#include "chartengine.h"

#include <QDateTime>
#include <QDebug>

ChartEngine::ChartEngine(int tex_radius, S52References* ref, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context)  {

  initializeOpenGLFunctions();

  _ready = false;
  _force_update = false;

  _context = context;

  _fbo = nullptr;
  assets = new S52Assets(context, ref);
  shaders = new ChartShaders(context);

  resize(tex_radius);
}

ChartEngine::~ChartEngine() {
  for (auto engine: area_engines)
    delete engine;
  for (auto engine: line_engines)
    delete engine;
  for (auto engine: text_engines)
    delete engine;
  for (auto engine: mark_engines)
    delete engine;

  delete _fbo;
  delete shaders;
  delete assets;
}


void ChartEngine::resize(int radius) {
  if (_fbo != nullptr && _fbo->width() == static_cast<int>(2*radius+1))
    return;

  _radius = radius;

  delete _fbo;

  QOpenGLFramebufferObjectFormat format;
  format.setAttachment(QOpenGLFramebufferObject::Depth);

  _fbo = new QOpenGLFramebufferObject(QSize(2*_radius+1, 2*_radius+1), format);

  _fbo->bind();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  _fbo->release();

  _force_update = true;
}


void ChartEngine::setChart(S52::Chart* chrt, S52References* ref) {
  _ready = false;

  clearChartData();

  setAreaLayers(chrt, ref);
  setLineLayers(chrt, ref);
  setTextLayers(chrt, ref);
  setMarkLayers(chrt, ref);
  setSndgLayer(chrt, ref);

  _ready = true;
  _force_update = true;
}


void ChartEngine::clearChartData() {
  for (auto engine: area_engines)
    delete engine;
  for (auto engine: line_engines)
    delete engine;
  for (auto engine: text_engines)
    delete engine;
  for (auto engine: mark_engines)
    delete engine;

  area_engines.clear();
  line_engines.clear();
  text_engines.clear();
  mark_engines.clear();

  _force_update = true;
}


void ChartEngine::setAreaLayers(S52::Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->areaLayerNames()) {
    S52::AreaLayer* layer = chrt->areaLayer(layer_name);
    ChartAreaEngine* engine = new ChartAreaEngine(_context);
    engine->setData(layer, assets, ref, layer->disp_prio[0]);
    area_engines.push_back(engine);
  }
}

void ChartEngine::setLineLayers(S52::Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->lineLayerNames()) {
    S52::LineLayer* layer = chrt->lineLayer(layer_name);
    ChartLineEngine* engine = new ChartLineEngine(_context);
    engine->setData(layer, assets, ref, 10 + layer->disp_prio[0]);
    line_engines.push_back(engine);
  }
}

void ChartEngine::setTextLayers(S52::Chart* chrt, S52References* ref) {
  Q_UNUSED(ref);

  for (QString layer_name : chrt->textLayerNames()) {
    S52::TextLayer* layer = chrt->textLayer(layer_name);
    ChartTextEngine* engine = new ChartTextEngine(_context);
    engine->setData(layer, 30);
    text_engines.push_back(engine);
  }
}

void ChartEngine::setMarkLayers(S52::Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->markLayerNames()) {
    S52::MarkLayer* layer = chrt->markLayer(layer_name);
    ChartMarkEngine* engine = new ChartMarkEngine(_context);
    engine->setData(layer, ref, 20 + layer->disp_prio[0]);
    mark_engines.push_back(engine);
  }
}

void ChartEngine::setSndgLayer(S52::Chart* chrt, S52References* ref) {
  ChartMarkEngine* engine = new ChartMarkEngine(_context);
  engine->setData(chrt->sndgLayer(), assets, ref, 100);
  mark_engines.push_back(engine);
}


void ChartEngine::update(const RLIState& state, const QString& color_scheme) {
  auto center = state.ship_position;
  double scale = state.chart_scale;
  double angle = state.north_shift;
  const QPoint& center_shift = state.center_shift;

  bool need_update = ( _force_update
                    || fabs(_center.lat - center.lat) > 0.000005
                    || fabs(_center.lon - center.lon) > 0.000005
                    || fabs(_scale - scale) > 0.005
                    || fabs(_angle - angle) > 0.005
                    || fabs(_center_shift.x() - center_shift.x()) > 0.005
                    || fabs(_center_shift.y() - center_shift.y()) > 0.005
                     );

  if (need_update) {
    _center = center;
    _scale = scale;
    _angle = angle;
    _center_shift = center_shift;

    draw(color_scheme);
  }
}


void ChartEngine::draw(const QString& color_scheme) {
  _fbo->bind();

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, _fbo->width(), _fbo->height());

  if (_ready) {
    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.ortho(0.f, _fbo->width(), 0.f, _fbo->height(), -1000.f, 1000.f);

    QMatrix4x4 transform;
    transform.setToIdentity();
    transform.translate(_center_shift.x() + _fbo->width()/2.f, _center_shift.y() + _fbo->height()/2.f, 0.f);

    drawAreaLayers(projection*transform, color_scheme);
    drawLineLayers(projection*transform, color_scheme);
    drawTextLayers(projection*transform);
    drawMarkLayers(projection*transform, color_scheme);
  }

  _fbo->release();
  _force_update = false;
}


void ChartEngine::drawAreaLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLShaderProgram* prog = shaders->getAreaProgram();

  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getAreaPatternTex(color_scheme);
  QOpenGLTexture* color_scheme_tex = assets->getColorSchemeTex(color_scheme);

  glUniform2f(shaders->getLineUnifLoc(COMMON_UNIF_CENTER), static_cast<GLfloat>(_center.lat), static_cast<GLfloat>(_center.lon));
  glUniform1f(shaders->getAreaUnifLoc(COMMON_UNIF_SCALE), static_cast<GLfloat>(_scale));
  glUniform1f(shaders->getAreaUnifLoc(COMMON_UNIF_NORTH), static_cast<GLfloat>(_angle));
  glUniform2f(shaders->getAreaUnifLoc(COMMON_UNIF_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getAreaUnifLoc(COMMON_UNIF_MVP_MATRIX), mvp_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pattern_tex->textureId());
  prog->setUniformValue(shaders->getAreaUnifLoc(COMMON_UNIF_PATTERN_TEX_ID), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, color_scheme_tex->textureId());
  prog->setUniformValue(shaders->getAreaUnifLoc(AREA_UNIF_COLOR_TABLE_TEX), 1);

  for (ChartAreaEngine* areaEngine : area_engines) {
    prog->setUniformValue(shaders->getAreaUnifLoc(COMMON_UNIF_DISPLAY_ORDER), static_cast<float>(areaEngine->displayOrder()));
    areaEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}


void ChartEngine::drawLineLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLTexture* pattern_tex = assets->getLinePatternTex(color_scheme);
  QOpenGLTexture* color_scheme_tex = assets->getColorSchemeTex(color_scheme);

  QOpenGLShaderProgram* prog = shaders->getLineProgram();
  prog->bind();

  glUniform2f(shaders->getLineUnifLoc(COMMON_UNIF_CENTER), static_cast<GLfloat>(_center.lat), static_cast<GLfloat>(_center.lon));
  glUniform1f(shaders->getLineUnifLoc(COMMON_UNIF_SCALE), static_cast<GLfloat>(_scale));
  glUniform1f(shaders->getLineUnifLoc(COMMON_UNIF_NORTH), static_cast<GLfloat>(_angle));
  glUniform2f(shaders->getLineUnifLoc(COMMON_UNIF_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getLineUnifLoc(COMMON_UNIF_MVP_MATRIX), mvp_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pattern_tex->textureId());
  glUniform1i(shaders->getLineUnifLoc(COMMON_UNIF_PATTERN_TEX_ID), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, color_scheme_tex->textureId());
  glUniform1i(shaders->getLineUnifLoc(LINE_UNIF_COLOR_TABLE_TEX), 1);

  for (ChartLineEngine* lineEngine : line_engines) {
    glUniform1f(shaders->getLineUnifLoc(COMMON_UNIF_DISPLAY_ORDER), lineEngine->displayOrder());
    lineEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}



void ChartEngine::drawTextLayers(const QMatrix4x4& mvp_matrix) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLTexture* font_tex = assets->getFontTexId();

  QOpenGLShaderProgram* prog = shaders->getTextProgram();
  prog->bind();

  glUniform2f(shaders->getLineUnifLoc(COMMON_UNIF_CENTER), static_cast<GLfloat>(_center.lat), static_cast<GLfloat>(_center.lon));
  glUniform1f(shaders->getTextUnifLoc(COMMON_UNIF_SCALE), static_cast<GLfloat>(_scale));
  glUniform1f(shaders->getTextUnifLoc(COMMON_UNIF_NORTH), static_cast<GLfloat>(_angle));
  prog->setUniformValue(shaders->getTextUnifLoc(COMMON_UNIF_MVP_MATRIX), mvp_matrix);

  glUniform1i(shaders->getTextUnifLoc(COMMON_UNIF_PATTERN_TEX_ID), 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font_tex->textureId());

  for (ChartTextEngine* textEngine : text_engines) {
    prog->setUniformValue(shaders->getTextUnifLoc(COMMON_UNIF_DISPLAY_ORDER), 0.f);
    textEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}



void ChartEngine::drawMarkLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLShaderProgram* prog = shaders->getMarkProgram();
  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getSymbolTex(color_scheme);

  glUniform2f(shaders->getLineUnifLoc(COMMON_UNIF_CENTER), static_cast<GLfloat>(_center.lat), static_cast<GLfloat>(_center.lon));
  glUniform1f(shaders->getMarkUnifLoc(COMMON_UNIF_SCALE), static_cast<GLfloat>(_scale));
  glUniform1f(shaders->getMarkUnifLoc(COMMON_UNIF_NORTH), static_cast<GLfloat>(_angle));
  glUniform2f(shaders->getMarkUnifLoc(COMMON_UNIF_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getMarkUnifLoc(COMMON_UNIF_MVP_MATRIX), mvp_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pattern_tex->textureId());

  glUniform1i(shaders->getMarkUnifLoc(COMMON_UNIF_PATTERN_TEX_ID), 0);

  for (ChartMarkEngine* markEngine : mark_engines) {
    prog->setUniformValue(shaders->getMarkUnifLoc(COMMON_UNIF_DISPLAY_ORDER), static_cast<float>(markEngine->displayOrder()));
    markEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}

