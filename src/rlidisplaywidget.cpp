#include "rlidisplaywidget.h"
#include "mainwindow.h"

#include <QOpenGLTexture>

#include <QDebug>
#include <QDateTime>
#include <QApplication>

#include "common/rliconfig.h"
#include "common/properties.h"

RLIDisplayWidget::RLIDisplayWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction start";

  _initialized = false;
  _debug_radar_tails_shift = 0;

  setMouseTracking(true);  

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction finish";
}

RLIDisplayWidget::~RLIDisplayWidget() {
  delete _chart_mngr;

  if (_initialized) {
    delete _infoFonts;
    delete _radarEngine;
    delete _tailsEngine;
    delete _maskEngine;
    delete _chartEngine;
    delete _menuEngine;

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
  if (name == "US2SP01M.000")
  //if (name == "CO200008.000")
    _chartEngine->setChart(_chart_mngr->getChart(name), _chart_mngr->refs());
}

void RLIDisplayWidget::debugInfo() {
  qDebug() << "";
  qDebug() << "Vendor: " << (const char*) glGetString(GL_VENDOR);
  qDebug() << "Renderer: " << (const char*) glGetString(GL_RENDERER);
  qDebug() << "OpenGL: " << (const char*) glGetString(GL_VERSION);
  qDebug() << "Shaders: " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
  /*
  qDebug() << "";
  qDebug() << "Extensions: ";
  QString exts((const char*) glGetString(GL_EXTENSIONS));
  for (QString ext : exts.split(" "))
     qDebug() << "\t" << ext;
  qDebug() << "";
  */
  int val;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
  qDebug() << "Max texture image units: " << val;
  qDebug() << "";
}

void RLIDisplayWidget::initializeGL() {
  if (_initialized)
    return;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init start";

  initializeOpenGLFunctions();

  debugInfo();

  glDisable(GL_STENCIL);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);

  _program = new QOpenGLShaderProgram(this);
  _chart_mngr = new ChartManager(this);

  const RLILayout* layout = RLIConfig::instance().currentLayout();

  uint peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  uint bearings_per_cycle  = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();

  uint circle_radius = layout->circle.radius;

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
  _maskEngine = new MaskEngine(size(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init start";
  _chartEngine = new ChartEngine(circle_radius, _chart_mngr->refs(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";
  _infoEngine = new InfoEngine(context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";
  _menuEngine = new MenuEngine(size(), layout->menu, context(), this);
  _menuEngine->setFonts(_infoFonts);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init finish";

  //-------------------------------------------------------------

  glGenBuffers(ATTR_COUNT, _vbo_ids);

  initShaders();

  emit initialized();
  _initialized = true;

  resizeGL(width(), height());

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init finish";


  _chart_mngr->loadCharts();
  connect(_chart_mngr, SIGNAL(new_chart_available(QString)), SLOT(onNewChartAvailable(QString)));
}

void RLIDisplayWidget::initShaders() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/main.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/main.frag.glsl");

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

  const RLILayout* layout = RLIConfig::instance().currentLayout();
  _radarEngine->resizeTexture(layout->circle.radius);
  _tailsEngine->resizeTexture(layout->circle.radius);
  _chartEngine->resize(layout->circle.radius);

  _maskEngine->resize(QSize(w, h));
   _menuEngine->resize(QSize(w, h), layout->menu);
}

void RLIDisplayWidget::paintGL() {
  QDateTime time = QDateTime::currentDateTime();

  if (frameTimes.size() == 0 || frameTimes.last().time().second() != time.time().second())
    emit secondChanged();

  frameTimes.push_back(time);
  while (frameTimes.size() > 20)
    frameTimes.removeFirst();

  if (!_initialized)
    return;

  paintLayers();
  updateLayers();
}

void RLIDisplayWidget::paintLayers() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const RLILayout* layout = RLIConfig::instance().currentLayout();

  glViewport(0, 0, width(), height());

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  QPoint shift(_debug_radar_tails_shift, _debug_radar_tails_shift);

  drawRect(QRect(layout->circle.boundRect.topLeft().toPoint(), _chartEngine->size()), _chartEngine->textureId());

  drawRect(QRect(layout->circle.boundRect.topLeft().toPoint() + shift, _radarEngine->size()), _radarEngine->textureId());
  drawRect(QRect(layout->circle.boundRect.topLeft().toPoint() - shift, _tailsEngine->size()), _tailsEngine->textureId());

  drawRect(rect(), _maskEngine->textureId());

  for (int i = 0; i < _infoEngine->blockCount(); i++)
    drawRect(_infoEngine->blockGeometry(i), _infoEngine->blockTextId(i));

  drawRect(QRect(_menuEngine->position(), _menuEngine->size()), _menuEngine->texture());

  glFlush();
}

void RLIDisplayWidget::updateLayers() {
  _radarEngine->updateTexture();
  _tailsEngine->updateTexture();

  std::pair<float, float> shipPosition = RLIState::instance().shipPosition();
  float chartScale = RLIState::instance().chartScale();

  QString colorScheme = _chart_mngr->refs()->getColorScheme();
  _chartEngine->update(shipPosition, chartScale , 0.f,  QPoint(0.f, 0.f), colorScheme);

  _infoEngine->update(_infoFonts);
  _menuEngine->update();
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


void RLIDisplayWidget::onMenuToggled() {
  if (_menuEngine->state() == MenuEngine::MAIN)
    _menuEngine->setState(MenuEngine::DISABLED);
  else
    _menuEngine->setState(MenuEngine::MAIN);
}

void RLIDisplayWidget::onConfigMenuToggled() {
  if (_menuEngine->state() == MenuEngine::CONFIG)
    _menuEngine->setState(MenuEngine::DISABLED);
  else
    _menuEngine->setState(MenuEngine::CONFIG);
}

void RLIDisplayWidget::onUpToggled() {
  if (_menuEngine->visible())
    _menuEngine->onUp();
}

void RLIDisplayWidget::onDownToggled() {
  if (_menuEngine->visible())
    _menuEngine->onDown();
}

void RLIDisplayWidget::onEnterToggled() {
  if (_menuEngine->visible())
    _menuEngine->onEnter();
}

void RLIDisplayWidget::onBackToggled() {
  if (_menuEngine->visible())
    _menuEngine->onBack();
}
