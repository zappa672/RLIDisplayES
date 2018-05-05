#include "chartengine.h"

#include <QDateTime>
#include <QDebug>

ChartEngine::ChartEngine(uint tex_radius, S52References* ref, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context)  {

  initializeOpenGLFunctions();

  _ready = false;
  _force_update = false;

  _context = context;

  _center_shift = QPoint(0, 0);
  _center = std::pair<float, float>(0, 0);
  _scale = 10;
  _angle = 0;

  _fbo = nullptr;
  settings = new ChartSettingsModel("chart_disp_conf.xml");
  assets = new S52Assets(context, ref);
  shaders = new ChartShaders(context);

  resize(tex_radius);
}

ChartEngine::~ChartEngine() {
  for (ChartAreaEngine* engine : area_engines)
    delete engine;
  //for (ChartLineEngine* engine : line_engines)
  //  delete engine;
  for (ChartTextEngine* engine : text_engines)
    delete engine;
  for (ChartMarkEngine* engine : mark_engines)
    delete engine;

  delete _fbo;
  delete shaders;
  delete assets;
}


void ChartEngine::resize(uint radius) {
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


void ChartEngine::setChart(S52Chart* chrt, S52References* ref) {
  _ready = false;

  clearChartData();

  setAreaLayers(chrt, ref);
  //setLineLayers(chrt, ref);
  setTextLayers(chrt, ref);
  setMarkLayers(chrt, ref);
  setSndgLayer(chrt, ref);

  _ready = true;
  _force_update = true;
}


void ChartEngine::clearChartData() {
  for (ChartAreaEngine* engine : area_engines)
    engine->clearData();
  /*
  for (ChartLineEngine* engine : line_engines)
    engine->clearData();
  */
  for (ChartTextEngine* engine : text_engines)
    engine->clearData();
  for (ChartMarkEngine* engine : mark_engines)
    engine->clearData();

  _force_update = true;
}


void ChartEngine::setAreaLayers(S52Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->getAreaLayerNames()) {
    S52AreaLayer* layer = chrt->getAreaLayer(layer_name);
    ChartLayerDisplaySettings clds = settings->layerSettings(layer_name);

    if (!clds.visible)
      continue;

    if (!area_engines.contains(layer_name))
      area_engines[layer_name] = new ChartAreaEngine(_context);

    area_engines[layer_name]->setData(layer, assets, ref, clds.order);
  }
}
/*
void ChartEngine::setLineLayers(S52Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->getLineLayerNames()) {
    S52LineLayer* layer = chrt->getLineLayer(layer_name);
    ChartLayerDisplaySettings clds = settings->layerSettings(layer_name);

    if (!clds.visible)
      continue;

    if (!line_engines.contains(layer_name))
      line_engines[layer_name] = new ChartLineEngine(_context);

    line_engines[layer_name]->setData(layer, assets, ref, clds.order);
  }
}
*/
void ChartEngine::setTextLayers(S52Chart* chrt, S52References* ref) {
  Q_UNUSED(ref);

  for (QString layer_name : chrt->getTextLayerNames()) {
    S52TextLayer* layer = chrt->getTextLayer(layer_name);
    ChartLayerDisplaySettings clds = settings->layerSettings(layer_name);

    if (!clds.visible)
      continue;

    if (!text_engines.contains(layer_name))
      text_engines[layer_name] = new ChartTextEngine(_context);

    text_engines[layer_name]->setData(layer, clds.order);
  }
}

void ChartEngine::setMarkLayers(S52Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->getMarkLayerNames()) {
    S52MarkLayer* layer = chrt->getMarkLayer(layer_name);
    ChartLayerDisplaySettings clds = settings->layerSettings(layer_name);

    if (!clds.visible)
      continue;

    if (!mark_engines.contains(layer_name))
      mark_engines[layer_name] = new ChartMarkEngine(_context);

    mark_engines[layer_name]->setData(layer, ref, clds.order);
  }
}

void ChartEngine::setSndgLayer(S52Chart* chrt, S52References* ref) {
  if (!mark_engines.contains("SOUNDG"))
    mark_engines["SOUNDG"] = new ChartMarkEngine(_context);

  mark_engines["SOUNDG"]->setData(chrt->getSndgLayer(), assets, ref, 1000);
}


void ChartEngine::update(std::pair<float, float> center, float scale, float angle, QPoint center_shift, const QString& color_scheme) {
  bool need_update = ( _force_update
                    || fabs(_center.first - center.first) > 0.000005
                    || fabs(_center.second - center.second) > 0.000005
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
    //drawLineLayers(projection*transform, color_scheme);
    drawTextLayers(projection*transform);
    drawMarkLayers(projection*transform, color_scheme);
  }

  _fbo->release();
  _force_update = false;
}


void ChartEngine::drawAreaLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLShaderProgram* prog = shaders->getChartAreaProgram();

  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getAreaPatternTex(color_scheme);
  QOpenGLTexture* color_scheme_tex = assets->getColorSchemeTex(color_scheme);

  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pattern_tex->textureId());
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, color_scheme_tex->textureId());
  prog->setUniformValue(shaders->getAreaUniformLoc(AREA_UNIFORMS_COLOR_TABLE_TEX), 1);

  for (ChartAreaEngine* areaEngine : area_engines) {
    prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_DISPLAY_ORDER), static_cast<float>(areaEngine->displayOrder()));
    areaEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}

/*
void ChartEngine::drawLineLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLTexture* pattern_tex = assets->getLinePatternTex(color_scheme);
  QOpenGLTexture* color_scheme_tex = assets->getColorSchemeTex(color_scheme);

  QOpenGLShaderProgram* prog = shaders->getChartLineProgram();
  prog->bind();

  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pattern_tex->textureId());
  glUniform1i(shaders->getLineUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, color_scheme_tex->textureId());
  glUniform1i(shaders->getLineUniformLoc(LINE_UNIFORMS_COLOR_TABLE_TEX), 1);

  for (ChartLineEngine* lineEngine : line_engines) {
    glUniform1f(shaders->getLineUniformLoc(COMMON_UNIFORMS_DISPLAY_ORDER), lineEngine->displayOrder());
    lineEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}
*/

void ChartEngine::drawTextLayers(const QMatrix4x4& mvp_matrix) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLTexture* font_tex = assets->getFontTexId();

  QOpenGLShaderProgram* prog = shaders->getChartTextProgram();
  prog->bind();

  glUniform2f(shaders->getTextUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  glUniform1f(shaders->getTextUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  glUniform1f(shaders->getTextUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  prog->setUniformValue(shaders->getTextUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  glUniform1i(shaders->getTextUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font_tex->textureId());

  for (ChartTextEngine* textEngine : text_engines) {
    prog->setUniformValue(shaders->getTextUniformLoc(COMMON_UNIFORMS_DISPLAY_ORDER), 0.f);
    textEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}

void ChartEngine::drawMarkLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  QOpenGLShaderProgram* prog = shaders->getChartMarkProgram();
  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getSymbolTex(color_scheme);

  glUniform2f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  glUniform1f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  glUniform1f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  glUniform2f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getMarkUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pattern_tex->textureId());

  glUniform1i(shaders->getMarkUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  for (ChartMarkEngine* markEngine : mark_engines) {
    prog->setUniformValue(shaders->getMarkUniformLoc(COMMON_UNIFORMS_DISPLAY_ORDER), static_cast<float>(markEngine->displayOrder()));
    markEngine->draw(shaders);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  prog->release();
}

