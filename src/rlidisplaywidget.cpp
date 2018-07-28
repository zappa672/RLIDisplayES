#include "rlidisplaywidget.h"
#include "mainwindow.h"

#include <QOpenGLTexture>

#include <QDebug>
#include <QDateTime>
#include <QApplication>

#include "common/properties.h"
#include "common/rlistrings.h"

RLIDisplayWidget::RLIDisplayWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction start";

  qRegisterMetaType<RLIString>("RLIString");

  _initialized = false;
  _debug_radar_tails_shift = 0;

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
    delete _ctrlEngine;

    delete _program;
  }
}

void RLIDisplayWidget::toggleRadarTailsShift() {
  if (_debug_radar_tails_shift == 0)
    _debug_radar_tails_shift = 320;
  else
    _debug_radar_tails_shift = 0;
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

  connect(_menuEngine, SIGNAL(radarBrightnessChanged(int)), _radarEngine, SLOT(onBrightnessChanged(int)));
  connect(_menuEngine, SIGNAL(languageChanged(RLIString)), _menuEngine, SLOT(onLanguageChanged(RLIString)));
  connect(_menuEngine, SIGNAL(languageChanged(RLIString)), _infoEngine, SLOT(onLanguageChanged(RLIString)));
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


  QPoint shift(_debug_radar_tails_shift, _debug_radar_tails_shift);
  QPoint topLeft = _layout_manager->layout()->circle.bounding_rect.topLeft();


  drawRect(QRect(topLeft, _chartEngine->size()), _chartEngine->textureId());

  drawRect(QRect(topLeft + shift, _radarEngine->size()), _radarEngine->textureId());
  drawRect(QRect(topLeft - shift, _tailsEngine->size()), _tailsEngine->textureId());


  QPointF center = _layout_manager->layout()->circle.center;

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, width(), height(), 0.f, -1.f, 1.f);

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate(center.x(), center.y(), 0.f);

  _trgtEngine->draw(projection*transform, _state);
  _ctrlEngine->draw(projection*transform);


  drawRect(rect(), _maskEngine->textureId());

  for (InfoBlock* block : _infoEngine->blocks())
    drawRect(block->geometry(), block->fbo()->texture());

  drawRect(_menuEngine->geometry(), _menuEngine->texture());

  if (_magnEngine->visible())
    drawRect(_magnEngine->geometry(), _magnEngine->texture());
}


void RLIDisplayWidget::updateLayers() {
  _radarEngine->updateTexture();
  _tailsEngine->updateTexture();

  QString colorScheme = _chart_mngr->refs()->getColorScheme();
  _chartEngine->update(_state, colorScheme);
  _infoEngine->update(_infoFonts);
  _menuEngine->update();
  _magnEngine->update( _radarEngine->amplitutedesVboId()
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

  _program->setUniformValue("texture", 0);
  _program->setUniformValue("mvp_matrix", _projection);

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




void RLIDisplayWidget::keyReleaseEvent(QKeyEvent *event) {
  pressedKeys.remove(event->key());
  QOpenGLWidget::keyReleaseEvent(event);
}

void RLIDisplayWidget::keyPressEvent(QKeyEvent *event) {

  switch(event->key()) {
  case Qt::Key_PageUp:
    /*
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged(qMin(_gain_ctrl->value() + 5, 255));

    if (mod_keys & Qt::AltModifier)
      emit waterChanged(qMin(_water_ctrl->value() + 5, 255));

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged(qMin(_rain_ctrl->value() + 5, 255));
    */
    break;

  case Qt::Key_PageDown:
    /*
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged(qMin(_gain_ctrl->value() - 5, 255));

    if (mod_keys & Qt::AltModifier)
      emit waterChanged(qMin(_water_ctrl->value() - 5, 255));

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged(qMin(_rain_ctrl->value() - 5, 255));
    */
    break;

  // Под. имп. Помех
  case Qt::Key_S:
    break;

  // Меню
  case Qt::Key_W:
    if(pressedKeys.contains(Qt::Key_B)) {
      if (_magnEngine->visible())
        return;

      if (_menuEngine->state() == MenuEngine::CONFIG)
        _menuEngine->setState(MenuEngine::DISABLED);
      else
        _menuEngine->setState(MenuEngine::CONFIG);
     } else {
      if (_magnEngine->visible())
        return;

      if (_menuEngine->state() == MenuEngine::MAIN)
        _menuEngine->setState(MenuEngine::DISABLED);
      else
        _menuEngine->setState(MenuEngine::MAIN);
     }
     break;

  // Шкала +
  case Qt::Key_Plus:
    _state.setChartScale( 0.95 * _state.chartScale() );
    break;

  // Шкала -
  case Qt::Key_Minus:
    _state.setChartScale( 1.05 * _state.chartScale() );
    break;

  // Вынос центра
  case Qt::Key_C:
    break;

  // Скрытое меню
  case Qt::Key_U:
    break;

  // Следы точки
  case Qt::Key_T:
    break;

  // Выбор цели
  case Qt::Key_Up:
    if (_menuEngine->visible())
      _menuEngine->onUp();
    break;

  // ЛИД / ЛОД
  case Qt::Key_Down:
    if (_menuEngine->visible())
      _menuEngine->onDown();
    break;

  case Qt::Key_Left:
    break;

  case Qt::Key_Right:
    break;

  // Захват
  case Qt::Key_Enter:
    if (_menuEngine->visible())
      _menuEngine->onEnter();
    break;

  //Сброс
  case Qt::Key_Escape:
    if (_menuEngine->visible())
      _menuEngine->onBack();
    break;

  // Парал. Линии
  case Qt::Key_Backslash:
    _ctrlEngine->setParallelLinesVisible(!_ctrlEngine->isParallelLinesVisible());
    break;

  //Электронная лупа
  case Qt::Key_L:
    if (!_menuEngine->visible())
      _magnEngine->setVisible(!_magnEngine->visible());
    break;

  //Обзор
  case Qt::Key_X:
    toggleRadarTailsShift();
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
    _ctrlEngine->setCirclesVisible(!_ctrlEngine->isCirclesVisible());
    break;

  }

  pressedKeys.insert(event->key());
  QOpenGLWidget::keyPressEvent(event);
}
