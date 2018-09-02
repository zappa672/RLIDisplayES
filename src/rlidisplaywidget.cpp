#include "rlidisplaywidget.h"
#include "mainwindow.h"

#include <math.h>

#include <QOpenGLTexture>

#include <QDebug>
#include <QDateTime>
#include <QApplication>

#include "common/properties.h"
#include "common/rlistrings.h"
#include "common/rlimath.h"


RLIDisplayWidget::RLIDisplayWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction start";

  qRegisterMetaType<RadarTarget>("RadarTarget");
  qRegisterMetaType<RLIString>("RLIString");

  _initialized = false;

  _chart_mngr = new ChartManager(this);
  _layout_manager = new RLILayoutManager("layouts.xml");

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction finish";
}


RLIDisplayWidget::~RLIDisplayWidget() {
  delete _chart_mngr;
  delete _layout_manager;

  if (_initialized) {
    delete _infoFonts;
    delete _radarEngine;
    delete _tailsEngine;
    delete _maskEngine;
    delete _chartEngine;
    delete _menuEngine;
    delete _magnEngine;
    delete _trgtEngine;
    delete _routeEngine;
    delete _ctrlEngine;

    delete _program;
  }
}


void RLIDisplayWidget::setupRadarDataSource(RadarDataSource* rds) {
  connect( rds, SIGNAL(updateData(uint,uint,GLfloat*))
         , _radarEngine, SLOT(updateData(uint,uint,GLfloat*)));

  connect( rds, SIGNAL(updateData2(uint,uint,GLfloat*))
         , _tailsEngine, SLOT(updateData(uint,uint,GLfloat*)));
}

void RLIDisplayWidget::setupTargetDataSource(TargetDataSource* tds) {
  connect( tds, SIGNAL(updateTarget(QString,RadarTarget))
         , _trgtEngine, SLOT(updateTarget(QString,RadarTarget)));
}

void RLIDisplayWidget::setupShipDataSource(ShipDataSource* sds) {
  _infoEngine->onPositionChanged(sds->position());
  _state.onShipPositionChanged(sds->position());

  connect( sds, SIGNAL(positionChanged(QVector2D))
         , _infoEngine , SLOT(onShipPositionChanged(QVector2D)));
  connect( sds, SIGNAL(positionChanged(QVector2D))
         ,  &_state, SLOT(onShipPositionChanged(QVector2D)));
}

void RLIDisplayWidget::onNewChartAvailable(const QString& name) {
  if (name == "US2SP01M.000") {
  //if (name == "CO200008.000") {
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Setting up chart " << name;
    _chartEngine->setChart(_chart_mngr->getChart(name), _chart_mngr->refs());
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Setting up chart finished";
  }
}

void RLIDisplayWidget::debugInfo() {
  qDebug() << "";
  qDebug() << "Vendor: " << (const char*) glGetString(GL_VENDOR);
  qDebug() << "Renderer: " << (const char*) glGetString(GL_RENDERER);
  qDebug() << "OpenGL: " << (const char*) glGetString(GL_VERSION);
  qDebug() << "Shaders: " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);

//  qDebug() << "";
//  qDebug() << "Extensions: ";
//  QString exts((const char*) glGetString(GL_EXTENSIONS));
//  for (QString ext : exts.split(" "))
//     qDebug() << "\t" << ext;
//  qDebug() << "";

//  int val;
//  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
//  qDebug() << "Max texture image units: " << val;
//  qDebug() << "";
}

void RLIDisplayWidget::initializeGL() {
  if (_initialized)
    return;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init start";

  initializeOpenGLFunctions();

  debugInfo();

  glDisable(GL_STENCIL);
  glDisable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);


  _program = new QOpenGLShaderProgram(this);

  uint peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  uint bearings_per_cycle  = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();

  uint circle_radius = _layout_manager->layout()->circle.radius;

  // Layers initialization
  //-------------------------------------------------------------

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Fonts init start";
  _infoFonts = new InfoFonts(context(), "data/textures/fonts");
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Fonts init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init start";
  _radarEngine = new RadarEngine(bearings_per_cycle, peleng_size, circle_radius, context(), this);  
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Tails engine init start";
  _tailsEngine = new RadarEngine(bearings_per_cycle, peleng_size, circle_radius, context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Tails engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init start";
  _maskEngine = new MaskEngine(size(), _layout_manager->layout()->circle, _infoFonts, context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init start";
  _chartEngine = new ChartEngine(circle_radius, _chart_mngr->refs(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";
  _infoEngine = new InfoEngine(_layout_manager->layout(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";
  _menuEngine = new MenuEngine(_layout_manager->layout()->menu, context(), this);
  _menuEngine->setFonts(_infoFonts);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";
  _magnEngine = new MagnifierEngine(_layout_manager->layout()->magnifier, context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Target engine init start";
  _trgtEngine = new TargetEngine(context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Target engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Route engine init start";
  _routeEngine = new RouteEngine(context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Route engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Controls engine init start";
  _ctrlEngine = new ControlsEngine(context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Controls engine init finish";

  //-------------------------------------------------------------

  glGenBuffers(ATTR_COUNT, _vbo_ids);
  initShaders();

  emit initialized();
  _initialized = true;

  resizeGL(width(), height());

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init finish";


  _chart_mngr->loadCharts();
  connect(_chart_mngr, SIGNAL(new_chart_available(QString)), SLOT(onNewChartAvailable(QString)));

  connect( _menuEngine, SIGNAL(radarBrightnessChanged(int))
         , _radarEngine, SLOT(onBrightnessChanged(int)));

  connect( _menuEngine, SIGNAL(languageChanged(RLIString))
         , _menuEngine, SLOT(onLanguageChanged(RLIString)));
  connect( _menuEngine, SIGNAL(languageChanged(RLIString))
         , _infoEngine, SLOT(onLanguageChanged(RLIString)));

  connect( _trgtEngine, SIGNAL(targetCountChanged(int))
         , _infoEngine, SLOT(onTargetCountChanged(int)));
  connect( _trgtEngine, SIGNAL(selectedTargetUpdated(QString,RadarTarget))
         , _infoEngine, SLOT(onSelectedTargetUpdated(QString,RadarTarget)));
}

void RLIDisplayWidget::initShaders() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "main.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "main.frag.glsl");

  _program->link();
  _program->bind();

  _unif_locs[UNIF_TEXTURE] = _program->uniformLocation("texture");
  _unif_locs[UNIF_MVPMATRIX] = _program->uniformLocation("mvp_matrix");

  _attr_locs[ATTR_POSITION] = _program->attributeLocation("a_position");
  _attr_locs[ATTR_TEXCOORD] = _program->attributeLocation("a_texcoord");

  _program->release();
}


void RLIDisplayWidget::resizeGL(int w, int h) {
  _projection.setToIdentity();
  _projection.ortho(geometry());

  if (!_initialized)
    return;

  QSize curr_size = _layout_manager->currentSize();
  _layout_manager->resize(QSize(w, h));
  QSize new_size = _layout_manager->currentSize();

  if (curr_size == new_size)
    return;

  int circle_radius = _layout_manager->layout()->circle.radius;

  _radarEngine->resizeTexture(circle_radius);
  _tailsEngine->resizeTexture(circle_radius);
  _chartEngine->resize(circle_radius);

  _maskEngine->resize(QSize(w, h), _layout_manager->layout()->circle);
  _menuEngine->resize(_layout_manager->layout()->menu);
  _magnEngine->resize(_layout_manager->layout()->magnifier);
  _infoEngine->resize(_layout_manager->layout());

  _infoEngine->secondChanged();
  _infoEngine->setFps(frameRate());
  _infoEngine->onPositionChanged(_state.shipPosition());
  _infoEngine->onTargetCountChanged(_trgtEngine->targetCount());
  _infoEngine->onSelectedTargetUpdated(_trgtEngine->selectedTag(), _trgtEngine->selectedTrgt());

  _infoEngine->updateGain(_state.gain());
  _infoEngine->updateWater(_state.water());
  _infoEngine->updateRain(_state.rain());
  _infoEngine->updateApch(_state.apch());
  _infoEngine->updateEmission(_state.emission());
}


float RLIDisplayWidget::frameRate() {
  if (frameTimes.size() < 2)
    return 0.f;

  QDateTime f = frameTimes.first();
  QDateTime l = frameTimes.last();
  int count = frameTimes.size() - 1;

  return 1000.f / (f.msecsTo(l) / count);
}


void RLIDisplayWidget::paintGL() {
  QDateTime time = QDateTime::currentDateTime();

  if (frameTimes.size() == 0 || frameTimes.last().time().second() != time.time().second())
    _infoEngine->secondChanged();

  frameTimes.push_back(time);
  while (frameTimes.size() > 20)
    frameTimes.removeFirst();

  _infoEngine->setFps(frameRate());


  if (!_initialized)
    return;

  paintLayers();
  updateLayers();

  glFlush();
}


void RLIDisplayWidget::paintLayers() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  glViewport(0, 0, width(), height());

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);


  QPoint topLeft = _layout_manager->layout()->circle.bounding_rect.topLeft();


  drawRect(QRect(topLeft, _chartEngine->size()), _chartEngine->textureId());

  drawRect(QRect(topLeft, _radarEngine->size()), _radarEngine->textureId());
  drawRect(QRect(topLeft, _tailsEngine->size()), _tailsEngine->textureId());


  QPointF center = _layout_manager->layout()->circle.center;

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, width(), height(), 0.f, -1.f, 1.f);

  const QSize& sz = _layout_manager->currentSize();

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( center.x() + (width() - sz.width()) / 2
                     , center.y() + (height() - sz.height()) / 2
                     , 0.f);

  _trgtEngine->draw(projection*transform, _state);
  _ctrlEngine->draw(projection*transform, _state);
  _routeEngine->draw(projection*transform, _state);


  drawRect(rect(), _maskEngine->textureId());

  for (InfoBlock* block : _infoEngine->blocks())
    drawRect(block->geometry(), block->fbo()->texture());

  drawRect(_menuEngine->geometry(), _menuEngine->texture());

  if (_state.state() == RLIWidgetState::RLISTATE_MAGNIFIER)
    drawRect(_magnEngine->geometry(), _magnEngine->texture());
}


void RLIDisplayWidget::updateLayers() {
  _radarEngine->updateTexture();
  _tailsEngine->updateTexture();

  QString colorScheme = _chart_mngr->refs()->getColorScheme();
  _chartEngine->update(_state, colorScheme);
  _infoEngine->update(_infoFonts);
  _menuEngine->update();
  _magnEngine->update( _radarEngine->ampsVboId()
                     , _radarEngine->paletteTexId()
                     , _radarEngine->pelengLength()
                     , _radarEngine->pelengCount()
                     , 90       // min_pel
                     , 96 );    // min_rad
}


void RLIDisplayWidget::drawRect(const QRectF& rect, GLuint textureId) {
  GLfloat vertices[] =  { static_cast<float>(rect.left()),  static_cast<float>(rect.bottom())
                        , static_cast<float>(rect.left()),  static_cast<float>(rect.top())
                        , static_cast<float>(rect.right()), static_cast<float>(rect.bottom())
                        , static_cast<float>(rect.right()), static_cast<float>(rect.top()) };

  GLfloat texcoords[] = { 0.0f, 1.0f
                        , 0.0f, 0.0f
                        , 1.0f, 1.0f
                        , 1.0f, 0.0f };

  _program->bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);

  const QSize& sz = _layout_manager->currentSize();

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( (width() - sz.width()) / 2
                     , (height() - sz.height()) / 2
                     , 0.f);

  _program->setUniformValue("texture", 0);
  _program->setUniformValue("mvp_matrix", _projection*transform);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer( _attr_locs[ATTR_POSITION], 2,  GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_TEXCOORD]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
  glVertexAttribPointer( _attr_locs[ATTR_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_TEXCOORD]);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _program->release();
}




void RLIDisplayWidget::mousePressEvent(QMouseEvent* event) {
  auto selected_coords = RLIMath::pos_to_coords( _state.shipPosition()
                                               , _layout_manager->layout()->circle.center
                                               , event->pos()
                                               , _state.chartScale() );

  _trgtEngine->select(selected_coords, _state.chartScale());
}


/*
void RLIDisplayWidget::onGainChanged(float value) {
  _state.gain = value;
  _infoEngine->updateGain(_state.gain);
}

void RLIDisplayWidget::onWaterChanged(float value) {
  _state.water = value;
  _infoEngine->updateWater(_state.water);
}

void RLIDisplayWidget::onRainChanged(float value) {
  _state.rain = value;
  _infoEngine->updateRain(_state.rain);
}

void RLIDisplayWidget::onApchChanged(float value) {
  _state.apch = value;
  _infoEngine->updateApch(_state.apch);
}

void RLIDisplayWidget::onEmissionChanged(float value) {
  _state.emission = value;
  _infoEngine->updateEmission(_state.emission);
}
*/

void RLIDisplayWidget::keyReleaseEvent(QKeyEvent *event) {
  pressedKeys.remove(event->key());
  QOpenGLWidget::keyReleaseEvent(event);
}


void RLIDisplayWidget::keyPressEvent(QKeyEvent* event) {
  pressedKeys.insert(event->key());

  if (    _state.state() == RLIWidgetState::RLISTATE_MAIN_MENU
       || _state.state() == RLIWidgetState::RLISTATE_CONFIG_MENU )
    _menuEngine->onKeyPressed(event);
  else
    _state.onKeyPressed(event, pressedKeys);

  QOpenGLWidget::keyPressEvent(event);
}
