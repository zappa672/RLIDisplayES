#include "rlidisplaywidget.h"
#include "mainwindow.h"

#include <QtMath>

#include <QOpenGLTexture>

#include <QDebug>
#include <QDateTime>
#include <QApplication>

#include "common/properties.h"
#include "common/rlistrings.h"
#include "common/rlimath.h"


RLIDisplayWidget::RLIDisplayWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction start";

  qRegisterMetaType<RLITarget>("RadarTarget");
  qRegisterMetaType<RLIShipState>("RLIShipState");
  qRegisterMetaType<RLIString>("RLIString");

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction finish";
}

RLIDisplayWidget::~RLIDisplayWidget() {
  if (!_initialized)
    return;

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

  //for (auto tex : _mode_textures)
  //  tex->destroy();
}


void RLIDisplayWidget::setupRadarDataSource(RadarDataSource* rds) {
  connect( rds, SIGNAL(updateRadarData(int, int, GLfloat*))
         , _radarEngine, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  connect( rds, SIGNAL(updateTrailData(int, int, GLfloat*))
         , _tailsEngine, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );
}

void RLIDisplayWidget::setupTargetDataSource(TargetDataSource* tds) {
  connect( tds, SIGNAL(updateTarget(QString,RadarTarget))
         , _trgtEngine, SLOT(updateTarget(QString,RadarTarget)));
}

void RLIDisplayWidget::setupShipDataSource(ShipDataSource* sds) {
  onShipStateChanged(sds->shipState());

  connect( sds, SIGNAL(shipStateChanged(RLIShipState))
         , this, SLOT(onShipStateChanged(RLIShipState)));
}




void RLIDisplayWidget::onNewChartAvailable(const QString& name) {
  if (name == "US2SP01M.000") {
  //if (name == "CO200008.000") {
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Setting up chart " << name;
    _chartEngine->setChart(_chart_mngr.getChart(name), _chart_mngr.refs());
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Setting up chart finished";
  }
}

void RLIDisplayWidget::debugInfo() {
  qDebug() << "";
  qDebug() << "Vendor: " << reinterpret_cast<const char*>( glGetString(GL_VENDOR) );
  qDebug() << "Renderer: " << reinterpret_cast<const char*>( glGetString(GL_RENDERER) );
  qDebug() << "OpenGL: " << reinterpret_cast<const char*>( glGetString(GL_VERSION) );
  qDebug() << "Shaders: " << reinterpret_cast<const char*>( glGetString(GL_SHADING_LANGUAGE_VERSION) );

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

  int peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  int bearings_per_cycle  = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();

  int circle_radius = _layout_manager.layout()->circle.radius;

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
  _maskEngine = new MaskEngine(size(), _layout_manager.layout()->circle, _infoFonts, context(), _state, this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init start";
  _chartEngine = new ChartEngine(circle_radius, _chart_mngr.refs(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";
  _infoEngine = new InfoEngine(_layout_manager.layout(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";
  _menuEngine = new MenuEngine(_layout_manager.layout()->menu, context(), this);
  _menuEngine->setFonts(_infoFonts);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";
  _magnEngine = new MagnifierEngine(_layout_manager.layout()->magnifier, context(), this);
  _magnEngine->setAmplitudesVBOId(_radarEngine->ampsVboId());
  _magnEngine->setPalletteTextureId(_radarEngine->paletteTexId());
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
  initModeTextures(tr("data/textures/symbols/"));

  emit initialized();
  _initialized = true;

  resizeGL(width(), height());

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init finish";


  _chart_mngr.loadCharts();
  connect( &_chart_mngr, SIGNAL(new_chart_available(QString)), SLOT(onNewChartAvailable(QString)));

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

  connect ( _menuEngine, SIGNAL(saveRoute(int))
          , _routeEngine, SLOT(saveTo(int)));
  connect ( _menuEngine, SIGNAL(loadRoute(int))
          , _routeEngine, SLOT(loadFrom(int)));

  connect( _menuEngine, SIGNAL(startRouteEdit())
         , this, SLOT(onRouteEditionStarted()));
  connect( _menuEngine, SIGNAL(finishRouteEdit())
         , this, SLOT(onRouteEditionFinished()));
}

void RLIDisplayWidget::initShaders() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "main.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "main.frag.glsl");

  _program->link();
  _program->bind();

  _unif_locs[UNIF_TEXTURE]    = _program->uniformLocation("texture");
  _unif_locs[UNIF_MVPMATRIX]  = _program->uniformLocation("mvp_matrix");

  _attr_locs[ATTR_POSITION]   = _program->attributeLocation("a_position");
  _attr_locs[ATTR_TEXCOORD]   = _program->attributeLocation("a_texcoord");

  _program->release();
}

void RLIDisplayWidget::initModeTextures(const QString& path) {
  for (QString fName : QDir(path).entryList(QStringList { "*.png" })) {
    QString name = fName.right(fName.length() - fName.lastIndexOf("/") - 1).replace(".png", "");
    QImage img = QImage(path + fName);

    QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);

    tex->setMipLevels(1);
    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    tex->setData(img, QOpenGLTexture::DontGenerateMipMaps);

    _mode_textures.insert(name[0].toLatin1(), tex);
  }
}


void RLIDisplayWidget::resizeGL(int w, int h) {
  _projection.setToIdentity();
  _projection.ortho(geometry());

  if (!_initialized)
    return;

  QSize curr_size = _layout_manager.currentSize();
  _layout_manager.resize(QSize(w, h));
  QSize new_size = _layout_manager.currentSize();

  if (curr_size == new_size)
    return;

  int circle_radius = _layout_manager.layout()->circle.radius;

  _radarEngine->resizeTexture(circle_radius);
  _tailsEngine->resizeTexture(circle_radius);
  _chartEngine->resize(circle_radius);

  _maskEngine->resize(QSize(w, h), _layout_manager.layout()->circle, _state);
  _menuEngine->resize(_layout_manager.layout()->menu);
  _magnEngine->resize(_layout_manager.layout()->magnifier);
  _infoEngine->resize(_layout_manager.layout());

  _infoEngine->secondChanged();
  _infoEngine->setFps(static_cast<int>(frameRate()));
  _infoEngine->onPositionChanged(_state.ship_position);
  _infoEngine->onTargetCountChanged(_trgtEngine->targetCount());
  _infoEngine->onSelectedTargetUpdated(_trgtEngine->selectedTag(), _trgtEngine->selectedTrgt());

  _infoEngine->onScaleChanged(_state.radar_scale.getCurScale());
  _state.chart_scale = (_state.radar_scale.getCurScale()->len * RLIMath::MILE2METER) / _layout_manager.layout()->circle.radius;

  _infoEngine->updateGain(_state.gain);
  _infoEngine->updateWater(_state.water);
  _infoEngine->updateRain(_state.rain);
  _infoEngine->updateApch(_state.apch);
  _infoEngine->updateEmission(_state.emission);
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

  _infoEngine->setFps(static_cast<int>(frameRate()));


  if (!_initialized)
    return;

  updateLayers();
  glFlush();

  paintLayers();
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

  auto layout = _layout_manager.layout();

  QPoint topLeft = layout->circle.bounding_rect.topLeft();


  if (_state.orientation == RLIOrientation::RLIORIENT_NORTH)
    drawRect(QRect(topLeft, _chartEngine->size()), _chartEngine->textureId());

  drawRect(QRect(topLeft, _radarEngine->size()), _radarEngine->textureId());
  drawRect(QRect(topLeft, _tailsEngine->size()), _tailsEngine->textureId());


  QPointF center = layout->circle.center;

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, width(), height(), 0.f, -1.f, 1.f);

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( static_cast<float>(center.x() + _state.center_shift.x())
                     , static_cast<float>(center.y() + _state.center_shift.y())
                     , 0.f);

  _trgtEngine->draw(projection*transform, _state);
  _ctrlEngine->draw(projection*transform, _state, layout->circle);
  _routeEngine->draw(projection*transform, _state);


  drawRect(rect(), _maskEngine->textureId());

  for (InfoBlock* block: _infoEngine->blocks())
    drawRect(block->geometry(), block->fbo()->texture());

  drawRect(_menuEngine->geometry(), _menuEngine->texture());

  if (_state.state == RLIWidgetState::RLISTATE_MAGNIFIER)
    drawRect(_magnEngine->geometry(), _magnEngine->texture());

  QOpenGLTexture* tex = _mode_textures[static_cast<const char>(_state.mode)];
  drawRect( QRect( layout->circle.center + QPoint(-tex->width() / 2, layout->circle.mode_symb_shift)
                 , QSize(tex->width(), tex->height()) )
          , tex->textureId());
}


void RLIDisplayWidget::updateLayers() {
  _radarEngine->updateTexture(_state);
  _tailsEngine->updateTexture(_state);

  QString colorScheme = _chart_mngr.refs()->getColorScheme();
  _chartEngine->update(_state, colorScheme);
  _infoEngine->update(_infoFonts);
  _menuEngine->update();
  _maskEngine->update(_state, _layout_manager.layout()->circle, false);

  if (_state.state == RLIWidgetState::RLISTATE_MAGNIFIER)
    _magnEngine->update( _radarEngine->pelengLength()
                       , _radarEngine->pelengCount()
                       , _state.magn_min_peleng
                       , _state.magn_min_rad );
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

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( 0.f, 0.f, 0.f );

  _program->setUniformValue("texture", 0);
  _program->setUniformValue("mvp_matrix", _projection*transform);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer( _attr_locs[ATTR_POSITION], 2,  GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_TEXCOORD]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
  glVertexAttribPointer( _attr_locs[ATTR_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_TEXCOORD]);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _program->release();
}



void RLIDisplayWidget::onShipStateChanged(const RLIShipState& sst) {
  _state.ship_position  = sst.position;
  _state.ship_course    = sst.course;
  _state.ship_speed     = sst.speed;

  _state.north_shift    = sst.course;
  _state.vn_cu          = sst.course;

  _infoEngine->onPositionChanged(sst.position);
  _infoEngine->onCourseChanged(sst.course);
  _infoEngine->onVnChanged(_state);
}


void RLIDisplayWidget::mouseMoveEvent(QMouseEvent* event) {
  auto diff = event->pos() - _layout_manager.layout()->circle.center;
  QVector2D diffV(diff);
  if (diffV.length() < 0.66f * _layout_manager.layout()->circle.radius)
    _state.cursor_pos = diff;

  double bearing = RLIMath::degs(qAtan2(static_cast<double>(diffV.x()), static_cast<double>(-diffV.y())));

  if (bearing < 0 && _state.orientation == RLIOrientation::RLIORIENT_NORTH)
    bearing = 360 + bearing;

  _infoEngine->onCursorPosChanged( static_cast<double>(diffV.length()), bearing );
}

void RLIDisplayWidget::mousePressEvent(QMouseEvent* event) {
  auto coords = RLIMath::pos_to_coords( _state.ship_position
                                      , _layout_manager.layout()->circle.center
                                      , event->pos()
                                      , _state.chart_scale );
  _trgtEngine->select(coords, _state.chart_scale);
}



void RLIDisplayWidget::onGainChanged(float value) {
  _infoEngine->updateGain(_state.gain = value);
}

void RLIDisplayWidget::onWaterChanged(float value) {
  _infoEngine->updateWater(_state.water = value);
}

void RLIDisplayWidget::onRainChanged(float value) {
  _infoEngine->updateRain(_state.rain = value);
}

void RLIDisplayWidget::onApchChanged(float value) {
  _infoEngine->updateApch(_state.apch = value);
}

void RLIDisplayWidget::onEmissionChanged(float value) {
  _infoEngine->updateEmission(_state.emission = value);
}


void RLIDisplayWidget::onRouteEditionStarted() {
  _routeEngine->clearCurrentRoute();
  _routeEngine->addPointToCurrent(_state.ship_position);
  _state.visir_center_pos = _state.ship_position;
  _state.state = RLIWidgetState::RLISTATE_ROUTE_EDITION;
}

void RLIDisplayWidget::onRouteEditionFinished() {
  _state.state = RLIWidgetState::RLISTATE_MAIN_MENU;
}


void RLIDisplayWidget::keyReleaseEvent(QKeyEvent *event) {
  pressedKeys.remove(event->key());
  QOpenGLWidget::keyReleaseEvent(event);
}


void RLIDisplayWidget::keyPressEvent(QKeyEvent* event) {
  pressedKeys.insert(event->key());
  auto mod_keys = event->modifiers();

  if (   _state.state == RLIWidgetState::RLISTATE_MAIN_MENU
      || _state.state == RLIWidgetState::RLISTATE_CONFIG_MENU
      || _state.state == RLIWidgetState::RLISTATE_ROUTE_EDITION )
    _menuEngine->onKeyPressed(event);

  switch(event->key()) {
  case Qt::Key_PageUp:
    if (mod_keys & Qt::ControlModifier)
      emit _infoEngine->updateGain( _state.gain = qMin(_state.gain + 5.0f, 255.0f) );

    if (mod_keys & Qt::AltModifier)
      emit _infoEngine->updateWater( _state.water = qMin(_state.water + 5.0f, 255.0f) );

    if (mod_keys & Qt::ShiftModifier)
      emit _infoEngine->updateRain(  _state.rain = qMin(_state.rain + 5.0f, 255.0f) );

    break;

  case Qt::Key_PageDown:
    if (mod_keys & Qt::ControlModifier)
      emit _infoEngine->updateGain( _state.gain = qMax(_state.gain - 5.0f, 0.0f) );

    if (mod_keys & Qt::AltModifier)
      emit _infoEngine->updateWater( _state.water = qMax(_state.water - 5.0f, 0.0f) );

    if (mod_keys & Qt::ShiftModifier)
      emit _infoEngine->updateRain( _state.rain = qMax(_state.rain - 5.0f, 0.0f) );

    break;

  // Под. имп. Помех
  case Qt::Key_S:
    break;

  // Меню
  case Qt::Key_W:
    if(pressedKeys.contains(Qt::Key_B)) {
      if (_state.state == RLIWidgetState::RLISTATE_CONFIG_MENU)
        _state.state = RLIWidgetState::RLISTATE_DEFAULT;
      else if (_state.state == RLIWidgetState::RLISTATE_DEFAULT)
        _state.state = RLIWidgetState::RLISTATE_CONFIG_MENU;

    } else {
      if (_state.state == RLIWidgetState::RLISTATE_MAIN_MENU)
        _state.state = RLIWidgetState::RLISTATE_DEFAULT;
      else if (_state.state == RLIWidgetState::RLISTATE_DEFAULT)
        _state.state = RLIWidgetState::RLISTATE_MAIN_MENU;
    }

    _menuEngine->onStateChanged(_state.state);
    break;

  // Шкала +
  case Qt::Key_Plus:
    if (_state.state == RLIWidgetState::RLISTATE_ROUTE_EDITION) {
      QPointF pos = QPointF( sin(RLIMath::rads(_state.vn_p)) * _state.vd
                           ,-cos(RLIMath::rads(_state.vn_p)) * _state.vd );
      //float scale = (_rli_scale.len*RLIMath::MILE2METER) / _maskEngine->getRadius();
      GeoPos last_route_point = _routeEngine->getLastPoint();
      GeoPos cursor_coords = RLIMath::pos_to_coords( last_route_point, QPoint(0, 0), pos, _state.chart_scale);
      _state.visir_center_pos = cursor_coords;
      _routeEngine->addPointToCurrent(cursor_coords);

      break;
    }

    _state.radar_scale.prevScale();
    _infoEngine->onScaleChanged(_state.radar_scale.getCurScale());
    _state.chart_scale = (_state.radar_scale.getCurScale()->len * RLIMath::MILE2METER) / _layout_manager.layout()->circle.radius;
    break;

  // Шкала -
  case Qt::Key_Minus:
    if (_state.state == RLIWidgetState::RLISTATE_ROUTE_EDITION) {
      _routeEngine->removePointFromCurrent();
      _state.visir_center_pos = _routeEngine->getLastPoint();
      break;
    }

    _state.radar_scale.nextScale();
    _infoEngine->onScaleChanged(_state.radar_scale.getCurScale());
    _state.chart_scale = (_state.radar_scale.getCurScale()->len * RLIMath::MILE2METER) / _layout_manager.layout()->circle.radius;
    break;

  // Вынос центра
  case Qt::Key_C:
    _state.center_shift = _state.cursor_pos;
    break;

  // Скрытое меню
  case Qt::Key_U:
    break;

  // Следы точки
  case Qt::Key_T:
    break;

  // Выбор цели
  case Qt::Key_Up:
    if (mod_keys & Qt::ControlModifier) {
      if (_state.magn_min_rad + _state.magn_height < 800) {
        _state.magn_min_rad += 1;
      }
    } else {
      _state.vd += 1.0;
      _infoEngine->onVdChanged(_state);
    }
    break;

  // ЛИД / ЛОД
  case Qt::Key_Down:
    if (mod_keys & Qt::ControlModifier) {
      if (_state.magn_min_rad > 0) {
        _state.magn_min_rad -= 1;
      }
    } else {
      _state.vd = qMax(0.0, _state.vd - 1.0);
      _infoEngine->onVdChanged(_state);
    }
    break;

  case Qt::Key_Left:
    if (mod_keys & Qt::ControlModifier) {
    _state.magn_min_peleng = (4096 + _state.magn_min_peleng - 1) % 4096;
    } else if (mod_keys & Qt::AltModifier) {
      _state.vn_cu = fmod(_state.vn_cu - 1.0, 360.0);
    } else if (mod_keys & Qt::ShiftModifier) {
      _state.course_mark_angle = fmod(_state.course_mark_angle - 1.0, 360.0);
    } else {
      _state.vn_p = fmod(_state.vn_p - 1.0, 360.0);
      _infoEngine->onVnChanged(_state);
    }
    break;

  case Qt::Key_Right:
    if (mod_keys & Qt::ControlModifier) {
      _state.magn_min_peleng = (4096 + _state.magn_min_peleng + 1) % 4096;
    } else if (mod_keys & Qt::AltModifier) {
      _state.vn_cu = fmod(_state.vn_cu + 1.0, 360.0);
    } else if (mod_keys & Qt::ShiftModifier) {
      _state.course_mark_angle = fmod(_state.course_mark_angle + 1.0, 360.0);
    } else {
      _state.vn_p = fmod(_state.vn_p + 1.0, 360.0);
      _infoEngine->onVnChanged(_state);
    }
    break;

  // Захват
  case Qt::Key_Return:
  case Qt::Key_Enter:
    break;

  //Сброс
  case Qt::Key_Escape:
    break;

  // Парал. Линии
  case Qt::Key_Backslash:
    _state.show_parallel = !_state.show_parallel;
    break;

  //Электронная лупа
  case Qt::Key_L:
    if ( _state.state == RLIWidgetState::RLISTATE_DEFAULT )
      _state.state = RLIWidgetState::RLISTATE_MAGNIFIER;
    else if ( _state.state == RLIWidgetState::RLISTATE_MAGNIFIER )
      _state.state = RLIWidgetState::RLISTATE_DEFAULT;
    break;

  //Обзор
  case Qt::Key_X:
    break;

  //Узкий / Шир.
  case Qt::Key_Greater:
    break;

  //Накоп. Видео
  case Qt::Key_V:
    break;

  //Сброс АС
  case Qt::Key_Q:
    break;

  //Манёвр
  case Qt::Key_M:
    break;

  //Курс / Север / Курс стаб
  case Qt::Key_H:
    switch(_state.orientation) {
      case RLIOrientation::RLIORIENT_HEAD:
        _state.orientation = RLIOrientation::RLIORIENT_NORTH;
        _state.mode = RLIMode::RLIMODE_X;
        break;
      case RLIOrientation::RLIORIENT_NORTH:
        _state.orientation = RLIOrientation::RLIORIENT_COURSE;
        _state.mode = RLIMode::RLIMODE_S;
        break;
      case RLIOrientation::RLIORIENT_COURSE:
        _state.orientation = RLIOrientation::RLIORIENT_HEAD;
        _state.mode = RLIMode::RLIMODE_T;
        break;
    }
    _infoEngine->onOrientationChanged(_state.orientation);
    break;

  //ИД / ОД
  case Qt::Key_R:
    break;

  //НКД
  case Qt::Key_D:
    break;

  //Карта (Маршрут)
  case Qt::Key_A:
    break;

  //Выбор
  case Qt::Key_G:
    break;

  //Стоп-кадр
  case Qt::Key_F:
    break;

  //Откл. Звука
  case Qt::Key_B:
    break;

  //Откл. ОК
  case Qt::Key_K:
    break;

  //Вынос ВН/ВД
  case Qt::Key_Slash:
    _state.show_circles = !_state.show_circles;
    break;
  }

  QOpenGLWidget::keyPressEvent(event);
}
