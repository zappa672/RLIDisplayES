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

  setMouseTracking(true);

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "RLIDisplayWidget construction finish";
}

RLIDisplayWidget::~RLIDisplayWidget() {
  delete _chart_mngr;

  if (_initialized) {
    delete _radarEngine;
    delete _tailsEngine;

    delete _maskEngine;

    delete _chartEngine;

    delete _program;
  }
}

void RLIDisplayWidget::onNewChartAvailable(const QString& name) {
  if (name == "CO200008.000")
    _chartEngine->setChart(_chart_mngr->getChart(name), _chart_mngr->refs());
}

void RLIDisplayWidget::debugInfo() {
  qDebug() << "Vendor: " << (const char*) glGetString(GL_VENDOR);
  qDebug() << "Renderer: " << (const char*) glGetString(GL_RENDERER);
  qDebug() << "OpenGL: " << (const char*) glGetString(GL_VERSION);
  qDebug() << "Shaders: " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
  //qDebug() << "Extensions: " << (const char*) glGetString(GL_EXTENSIONS);
}

void RLIDisplayWidget::initializeGL() {
  if (_initialized)
    return;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init start";

  initializeOpenGLFunctions();

  debugInfo();

  glDisable(GL_STENCIL);
  glEnable(GL_TEXTURE_2D);

  _program = new QOpenGLShaderProgram(this);
  _chart_mngr = new ChartManager(this);


  const RLILayout* layout = RLIConfig::instance().currentLayout();

  uint peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  uint bearings_per_cycle  = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();

  uint circle_radius = layout->circle.radius;

  // Layers initialization
  //-------------------------------------------------------------

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init start";
  _radarEngine = new RadarEngine(bearings_per_cycle, peleng_size, circle_radius, context(), this);
  _tailsEngine = new RadarEngine(bearings_per_cycle, peleng_size, circle_radius, context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init finish";


  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init start";
  _maskEngine = new MaskEngine(size(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";


  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init start";
  _chartEngine = new ChartEngine(circle_radius, _chart_mngr->refs(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Chart engine init finish";

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

  _maskEngine->resize(QSize(w, h));
}

void RLIDisplayWidget::paintGL() {
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

  fillRectWithTexture( QRect(layout->circle.boundRect.topLeft().toPoint(), _chartEngine->size())
                     , _chartEngine->textureId());

  fillRectWithTexture( QRect(layout->circle.boundRect.topLeft().toPoint() - QPoint(456, 456), _radarEngine->size())
                     , _radarEngine->textureId());

  fillRectWithTexture( QRect(layout->circle.boundRect.topLeft().toPoint() + QPoint(456, 456), _tailsEngine->size())
                     , _tailsEngine->textureId());

  fillRectWithTexture( rect(), _maskEngine->textureId());

  glFlush();
}

void RLIDisplayWidget::updateLayers() {
  _radarEngine->updateTexture();
  _tailsEngine->updateTexture();

  _chartEngine->update(QVector2D(12.6443f, -81.6405f), 50.f , 0.f,  QPoint(0.f, 0.f));
}

void RLIDisplayWidget::fillRectWithTexture(const QRectF& rect, GLuint textureId) {
  GLfloat vertices[] =  { static_cast<float>(rect.left()),  static_cast<float>(rect.bottom())
                        , static_cast<float>(rect.left()),  static_cast<float>(rect.top())
                        , static_cast<float>(rect.right()), static_cast<float>(rect.top())
                        , static_cast<float>(rect.right()), static_cast<float>(rect.bottom()) };

  GLfloat texcoords[] = { 0.0f, 1.0f
                        , 0.0f, 0.0f
                        , 1.0f, 0.0f
                        , 1.0f, 1.0f };

  GLubyte indices[] =   { 0, 1, 2     // first triangle (bottom left - top left - top right)
                        , 0, 2, 3 };  // second triangle (bottom left - top right - bottom right)


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

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  _program->release();
}
