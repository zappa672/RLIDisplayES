#include "chartengine.h"

#include <QDateTime>
#include <QDebug>

ChartEngine::ChartEngine(uint tex_radius, S52References* ref, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context)  {

  initializeOpenGLFunctions();

  _context = context;

  _center_shift = QPoint(0, 0);
  _center = QVector2D(0, 0);
  _scale = 10;
  _angle = 0;

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
  if (_fbo != nullptr) {
    if (_fbo->width() == static_cast<int>(2*radius+1))
      return;

    delete _fbo;
  }

  _radius = radius;

  QOpenGLFramebufferObjectFormat format;
  format.setMipmap(true);
  format.setSamples(0);

  _fbo = new QOpenGLFramebufferObject(2*_radius+1, 2*_radius+1, format);
  draw();
}


void ChartEngine::setChart(S52Chart* chrt, S52References* ref) {
  clearChartData();

  setAreaLayers(chrt, ref);
  setLineLayers(chrt, ref);
  setMarkLayers(chrt, ref);
  setTextLayers(chrt, ref);

  setSndgLayer(chrt, ref);

  draw();
}

void ChartEngine::update(QVector2D center, float scale, float angle, QPoint center_shift) {
  bool need_update = (abs(_center.x() - center.x()) > 0.0005
                    || abs(_center.y() - center.y()) > 0.0005
                    || abs(_scale - scale) > 0.005
                    || abs(_angle - angle) > 0.005
                    || abs(_center_shift.x() - center_shift.x()) > 0.005
                    || abs(_center_shift.y() - center_shift.y()) > 0.005);

  if (need_update) {
    _center = center;
    _scale = scale;
    _angle = angle;
    _center_shift = center_shift;
    draw();
  }
}

void ChartEngine::draw() {
  glEnable(GL_BLEND);

  _fbo->bind();

  glClearColor(0.17f, 0.23f, 0.22f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, _fbo->width(), _fbo->height());

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, _fbo->width(), _fbo->height(), 0, -1, 1 );

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(_center_shift.x() + _fbo->width()/2.f, _center_shift.y() + _fbo->height()/2.f, 0);

  drawLayers();

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();

  glMatrixMode( GL_PROJECTION );
  glPopMatrix();

  _fbo->release();
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
}



void ChartEngine::drawLayers() {
  QStringList displayOrder = settings->getLayersDisplayOrder();
  for (int i = displayOrder.size() - 1; i >= 0; i--)
    if (!settings->isLayerVisible(displayOrder[i]))
      displayOrder.removeAt(i);


  shaders->getChartAreaProgram()->bind();
  for (int i = 0; i < displayOrder.size(); i++) {
    QString s = displayOrder[i];
    if (area_engines.contains(s) && area_engines[s] != NULL)
      area_engines[s]->draw(shaders, _center, _scale, _angle);
  }
  shaders->getChartAreaProgram()->release();


  shaders->getChartLineProgram()->bind();
  for (int i = 0; i < displayOrder.size(); i++) {
    QString s = displayOrder[i];
    if (line_engines.contains(s) && line_engines[s] != NULL)
      line_engines[s]->draw(shaders, _center, _scale, _angle);
  }
  shaders->getChartLineProgram()->release();


  shaders->getChartMarkProgram()->bind();
  for (int i = 0; i < displayOrder.size(); i++) {
    QString s = displayOrder[i];
    if (mark_engines.contains(s) && mark_engines[s] != NULL)
      mark_engines[s]->draw(shaders, _center, _scale, _angle);
  }
  shaders->getChartMarkProgram()->release();


  shaders->getChartTextProgram()->bind();
  for (int i = 0; i < displayOrder.size(); i++) {
    QString s = displayOrder[i];
    if (text_engines.contains(s) && text_engines[s] != NULL)
      text_engines[s]->draw(shaders, _center, _scale, _angle);
  }
  shaders->getChartTextProgram()->release();


  if (settings->areSoundingsVisible()) {
    shaders->getChartSndgProgram()->bind();
    sndg_engine->draw(shaders, _center, _scale, _angle);
    shaders->getChartSndgProgram()->release();
  }
}


void ChartEngine::setAreaLayers(S52Chart* chrt, S52References* ref) {
  QList<QString> layer_names = chrt->getAreaLayerNames();

  for (int i = 0; i < layer_names.size(); i++) {
    QString layer_name = layer_names[i];
    S52AreaLayer* layer = chrt->getAreaLayer(layer_name);

    if (!area_engines.contains(layer_name))
      area_engines[layer_name] = new ChartAreaEngine(_context);

    area_engines[layer_name]->setPatternTexture(assets->getPatternTextureId(ref->getColorScheme())
                                              , assets->getPatternTextureDim(ref->getColorScheme()));
    area_engines[layer_name]->setData(layer, assets, ref);
  }
}


void ChartEngine::setLineLayers(S52Chart* chrt, S52References* ref) {
  QList<QString> layer_names = chrt->getLineLayerNames();

  for (int i = 0; i < layer_names.size(); i++) {
    QString layer_name = layer_names[i];
    S52LineLayer* layer = chrt->getLineLayer(layer_name);

    if (!line_engines.contains(layer_name))
      line_engines[layer_name] = new ChartLineEngine(_context);

    line_engines[layer_name]->setPatternTexture(assets->getLineTextureId(ref->getColorScheme())
                                              , assets->getLineTextureDim(ref->getColorScheme()));
    line_engines[layer_name]->setData(layer, assets, ref);
  }
}

void ChartEngine::setMarkLayers(S52Chart* chrt, S52References* ref) {
  QList<QString> layer_names = chrt->getMarkLayerNames();

  for (int i = 0; i < layer_names.size(); i++) {
    QString layer_name = layer_names[i];
    S52MarkLayer* layer = chrt->getMarkLayer(layer_name);

    if (!mark_engines.contains(layer_name))
      mark_engines[layer_name] = new ChartMarkEngine(_context);

    mark_engines[layer_name]->setPatternTexture(assets->getSymbolTextureId(ref->getColorScheme())
                                              , assets->getSymbolTextureDim(ref->getColorScheme()));
    mark_engines[layer_name]->setData(layer, assets, ref);
  }
}

void ChartEngine::setTextLayers(S52Chart* chrt, S52References* ref) {
  Q_UNUSED(ref);

  QList<QString> layer_names = chrt->getTextLayerNames();

  for (int i = 0; i < layer_names.size(); i++) {
    QString layer_name = layer_names[i];
    S52TextLayer* layer = chrt->getTextLayer(layer_name);

    if (!text_engines.contains(layer_name))
      text_engines[layer_name] = new ChartTextEngine(_context);

    text_engines[layer_name]->setGlyphTexture(assets->getGlyphTextureId());
    text_engines[layer_name]->setData(layer);
  }
}

void ChartEngine::setSndgLayer(S52Chart* chrt, S52References* ref) {
  S52SndgLayer* layer = chrt->getSndgLayer();
  if (layer == NULL)
    return;

  sndg_engine->setPatternTexture(assets->getSymbolTextureId(ref->getColorScheme())
                               , assets->getSymbolTextureDim(ref->getColorScheme()));
  sndg_engine->setData(layer, assets, ref);
}
