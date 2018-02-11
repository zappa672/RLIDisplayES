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

  sndg_engine = new ChartSndgEngine(context);

  resize(tex_radius);
}

ChartEngine::~ChartEngine() {
  delete sndg_engine;

  QList<QString> layer_names;

  for (int i = 0; i < (layer_names = area_engines.keys()).size(); i++)
    delete area_engines[layer_names[i]];
  for (int i = 0; i < (layer_names = line_engines.keys()).size(); i++)
    delete line_engines[layer_names[i]];
  for (int i = 0; i < (layer_names = mark_engines.keys()).size(); i++)
    delete mark_engines[layer_names[i]];
  for (int i = 0; i < (layer_names = text_engines.keys()).size(); i++)
    delete text_engines[layer_names[i]];

  delete _fbo;
  delete shaders;
  delete assets;
}


void ChartEngine::resize(uint radius) {
  if (_fbo != nullptr && _fbo->width() == static_cast<int>(2*radius+1))
    return;

  _radius = radius;

  delete _fbo;
  _fbo = new QOpenGLFramebufferObject(QSize(2*_radius+1, 2*_radius+1));

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
  setLineLayers(chrt, ref);
  setMarkLayers(chrt, ref);
  setTextLayers(chrt, ref);
  setSndgLayer(chrt, ref);

  _ready = true;
  _force_update = true;
}


void ChartEngine::clearChartData() {
  sndg_engine->clearData();
  QList<QString> layer_names;

  for (int i = 0; i < (layer_names = area_engines.keys()).size(); i++)
    area_engines[layer_names[i]]->clearData();
  for (int i = 0; i < (layer_names = line_engines.keys()).size(); i++)
    line_engines[layer_names[i]]->clearData();
  for (int i = 0; i < (layer_names = mark_engines.keys()).size(); i++)
    mark_engines[layer_names[i]]->clearData();
  for (int i = 0; i < (layer_names = text_engines.keys()).size(); i++)
    text_engines[layer_names[i]]->clearData();

  _force_update = true;
}


void ChartEngine::setAreaLayers(S52Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->getAreaLayerNames()) {
    S52AreaLayer* layer = chrt->getAreaLayer(layer_name);
    if (!area_engines.contains(layer_name))
      area_engines[layer_name] = new ChartAreaEngine(_context);
    area_engines[layer_name]->setData(layer, assets, ref);
  }
}

void ChartEngine::setLineLayers(S52Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->getLineLayerNames()) {
    S52LineLayer* layer = chrt->getLineLayer(layer_name);
    if (!line_engines.contains(layer_name))
      line_engines[layer_name] = new ChartLineEngine(_context);
    line_engines[layer_name]->setData(layer, assets, ref);
  }
}

void ChartEngine::setTextLayers(S52Chart* chrt, S52References* ref) {
  Q_UNUSED(ref);

  for (QString layer_name : chrt->getTextLayerNames()) {
    S52TextLayer* layer = chrt->getTextLayer(layer_name);
    if (!text_engines.contains(layer_name))
      text_engines[layer_name] = new ChartTextEngine(_context);
    text_engines[layer_name]->setData(layer);
  }
}

void ChartEngine::setMarkLayers(S52Chart* chrt, S52References* ref) {
  for (QString layer_name : chrt->getMarkLayerNames()) {
    S52MarkLayer* layer = chrt->getMarkLayer(layer_name);
    if (!mark_engines.contains(layer_name))
      mark_engines[layer_name] = new ChartMarkEngine(_context);
    mark_engines[layer_name]->setData(layer, ref);
  }
}

void ChartEngine::setSndgLayer(S52Chart* chrt, S52References* ref) {
  S52SndgLayer* layer = chrt->getSndgLayer();
  if (layer != NULL)
    sndg_engine->setData(layer, assets, ref);
}


void ChartEngine::update(std::pair<float, float> center, float scale, float angle, QPoint center_shift, const QString& color_scheme) {
  bool need_update = ( _force_update
                    || fabs(_center.first - center.first) > 0.00005
                    || fabs(_center.second - center.second) > 0.00005
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
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH);
  glDisable(GL_DEPTH_TEST);

  _fbo->bind();

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, _fbo->width(), _fbo->height());

  if (_ready) {
    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.ortho(0, _fbo->width(), 0, _fbo->height(), -1, 1);

    QMatrix4x4 transform;
    transform.setToIdentity();
    transform.translate(_center_shift.x() + _fbo->width()/2.f, _center_shift.y() + _fbo->height()/2.f, 0.f);

    QStringList displayOrder = settings->getLayersDisplayOrder();
    for (int i = displayOrder.size() - 1; i >= 0; i--)
      if (!settings->isLayerVisible(displayOrder[i]))
        displayOrder.removeAt(i);

    drawAreaLayers(displayOrder, projection*transform, color_scheme);
    drawLineLayers(displayOrder, projection*transform, color_scheme);
    drawTextLayers(displayOrder, projection*transform);
    drawMarkLayers(displayOrder, projection*transform, color_scheme);

    drawSndgLayer(projection*transform, color_scheme);
  }

  _fbo->release();

  _force_update = false;
}


void ChartEngine::drawAreaLayers(const QStringList& displayOrder, const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  QOpenGLShaderProgram* prog = shaders->getChartAreaProgram();
  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getAreaPatternTex(color_scheme);
  QOpenGLTexture* color_scheme_tex = assets->getColorSchemeTex(color_scheme);

  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getAreaUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  pattern_tex->bind(0);
  glUniform1i(shaders->getAreaUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  color_scheme_tex->bind(1);
  glUniform1i(shaders->getAreaUniformLoc(AREA_UNIFORMS_COLOR_TABLE_TEX), 1);

  for (QString layer : displayOrder)
    if (area_engines.contains(layer) && area_engines[layer] != nullptr)
      area_engines[layer]->draw(shaders);

  pattern_tex->release(0);
  color_scheme_tex->release(1);

  prog->release();
}

void ChartEngine::drawLineLayers(const QStringList& displayOrder, const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  QOpenGLShaderProgram* prog = shaders->getChartLineProgram();
  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getLinePatternTex(color_scheme);
  QOpenGLTexture* color_scheme_tex = assets->getColorSchemeTex(color_scheme);

  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getLineUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  pattern_tex->bind(0);
  glUniform1i(shaders->getLineUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  color_scheme_tex->bind(1);
  glUniform1i(shaders->getLineUniformLoc(LINE_UNIFORMS_COLOR_TABLE_TEX), 1);

  for (QString layer : displayOrder)
    if (line_engines.contains(layer) && line_engines[layer] != nullptr)
      line_engines[layer]->draw(shaders);

  pattern_tex->release(0);
  color_scheme_tex->release(1);

  prog->release();
}

void ChartEngine::drawTextLayers(const QStringList& displayOrder, const QMatrix4x4& mvp_matrix) {
  QOpenGLShaderProgram* prog = shaders->getChartTextProgram();
  prog->bind();

  glUniform2f(shaders->getTextUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  glUniform1f(shaders->getTextUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  glUniform1f(shaders->getTextUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  prog->setUniformValue(shaders->getTextUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  QOpenGLTexture* glyph_tex = assets->getFontTexId();

  glyph_tex->bind(0);
  glUniform1i(shaders->getTextUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  for (QString layer : displayOrder)
    if (text_engines.contains(layer) && text_engines[layer] != nullptr)
      text_engines[layer]->draw(shaders);

  glyph_tex->release(0);

  prog->release();
}

void ChartEngine::drawMarkLayers(const QStringList& displayOrder, const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  QOpenGLShaderProgram* prog = shaders->getChartMarkProgram();
  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getSymbolTex(color_scheme);

  glUniform2f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  glUniform1f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  glUniform1f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  glUniform2f(shaders->getMarkUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getMarkUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  pattern_tex->bind(0);
  glUniform1i(shaders->getMarkUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  for (QString layer : displayOrder)
    if (mark_engines.contains(layer) && mark_engines[layer] != nullptr)
      mark_engines[layer]->draw(shaders);

  pattern_tex->release(0);

  prog->release();
}

void ChartEngine::drawSndgLayer(const QMatrix4x4& mvp_matrix, const QString& color_scheme) {
  QOpenGLShaderProgram* prog = shaders->getChartSndgProgram();
  prog->bind();

  QOpenGLTexture* pattern_tex = assets->getSymbolTex(color_scheme);

  glUniform2f(shaders->getSndgUniformLoc(COMMON_UNIFORMS_CENTER), _center.first, _center.second);
  glUniform1f(shaders->getSndgUniformLoc(COMMON_UNIFORMS_SCALE), _scale);
  glUniform1f(shaders->getSndgUniformLoc(COMMON_UNIFORMS_NORTH), _angle);
  glUniform2f(shaders->getSndgUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_DIM), pattern_tex->width(), pattern_tex->height());
  prog->setUniformValue(shaders->getSndgUniformLoc(COMMON_UNIFORMS_MVP_MATRIX), mvp_matrix);

  pattern_tex->bind(0);
  glUniform1i(shaders->getSndgUniformLoc(COMMON_UNIFORMS_PATTERN_TEX_ID), 0);

  sndg_engine->draw(shaders);

  pattern_tex->release(0);

  prog->release();
}
