#include "maskengine.h"

#include "../common/rliconfig.h"

static double const PI = acos(-1);

MaskEngine::MaskEngine(const QSize& sz, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {
  _fbo = NULL;

  initializeOpenGLFunctions();

  glGenBuffers(ATTR_CNT, _hole_vbo_ids);
  glGenBuffers(ATTR_CNT, _rect_vbo_ids);

  _fbo = new QOpenGLFramebufferObject(QSize(1, 1));
  _program = new QOpenGLShaderProgram();

  initShader();
  resize(sz);
}

MaskEngine::~MaskEngine() {
  delete _fbo;
  delete _program;

  glDeleteBuffers(ATTR_CNT, _hole_vbo_ids);
  glDeleteBuffers(ATTR_CNT, _rect_vbo_ids);
}

void MaskEngine::resize(const QSize& sz) {
  if (sz == _fbo->size())
    return;

  delete _fbo;
  _fbo = new QOpenGLFramebufferObject(sz);

  initBuffers();
  update();
}

void MaskEngine::update() {
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH);
  glDisable(GL_DEPTH_TEST);

  _fbo->bind();

  glViewport(0, 0, _fbo->width(), _fbo->height());

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(QRect(QPoint(0, 0), _fbo->size()));

  _program->bind();

  _program->setUniformValue(_unif_locs[UNIF_MVP], projection);


  // Draw background
  glBindBuffer(GL_ARRAY_BUFFER, _rect_vbo_ids[ATTR_POS]);
  glVertexAttribPointer( _attr_locs[ATTR_POS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_POS]);

  glBindBuffer(GL_ARRAY_BUFFER, _rect_vbo_ids[ATTR_CLR]);
  glVertexAttribPointer( _attr_locs[ATTR_CLR], 4, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_CLR]);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


  // Make transparent circle hole
  glBindBuffer(GL_ARRAY_BUFFER, _hole_vbo_ids[ATTR_POS]);
  glVertexAttribPointer( _attr_locs[ATTR_POS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_POS]);

  glBindBuffer(GL_ARRAY_BUFFER, _hole_vbo_ids[ATTR_CLR]);
  glVertexAttribPointer( _attr_locs[ATTR_CLR], 4, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_CLR]);

  glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_RAY_COUNT+2);


  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _program->release();

  _fbo->release();

  glFinish();
}





void MaskEngine::initShader() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/mask.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mask.frag.glsl");
  _program->link();
  _program->bind();

  _unif_locs[UNIF_MVP] = _program->uniformLocation("mvp_matrix");

  _attr_locs[ATTR_POS] = _program->attributeLocation("position");
  _attr_locs[ATTR_CLR] = _program->attributeLocation("color");

  _program->release();
}

void MaskEngine::initBuffers() {
  initHoleBuffers();
  initRectBuffers();
}

void MaskEngine::initRectBuffers() {
  GLfloat positions[] = { 0.f         , 0.f
                        , 0.f         , static_cast<GLfloat>(_fbo->height())
                        , static_cast<GLfloat>(_fbo->width()), static_cast<GLfloat>(_fbo->height())
                        , static_cast<GLfloat>(_fbo->width()), 0.f };

  GLfloat colors[] = { .04f, .05f, .04f, 1.0f
                     , .42f, .44f, .40f, 1.0f
                     , .42f, .44f, .40f, 1.0f
                     , .04f, .05f, .04f, 1.0f};

  glBindBuffer(GL_ARRAY_BUFFER, _rect_vbo_ids[ATTR_POS]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), positions, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _rect_vbo_ids[ATTR_CLR]);
  glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), colors, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MaskEngine::initHoleBuffers() {
  const RLILayout* layout = RLIConfig::instance().currentLayout();
  QPointF center = layout->circle.center;
  float radius = layout->circle.radius;

  std::vector<GLfloat> positions;
  std::vector<GLfloat> colors;

  positions.push_back(center.x());
  positions.push_back(center.y());
  colors.push_back(1.f);
  colors.push_back(1.f);
  colors.push_back(1.f);
  colors.push_back(0.f);

  for (int i = 0; i < CIRCLE_RAY_COUNT+1; i++) {
    float angle = 2 * PI * static_cast<float>(i) / CIRCLE_RAY_COUNT;
    float x = center.x() + radius*cos(angle);
    float y = center.y() + radius*sin(angle);

    positions.push_back(x);
    positions.push_back(y);
    colors.push_back(1.f);
    colors.push_back(1.f);
    colors.push_back(1.f);
    colors.push_back(0.f);
  }

  glBindBuffer(GL_ARRAY_BUFFER, _hole_vbo_ids[ATTR_POS]);
  glBufferData(GL_ARRAY_BUFFER, 2*(CIRCLE_RAY_COUNT+2)*sizeof(GLfloat), positions.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _hole_vbo_ids[ATTR_CLR]);
  glBufferData(GL_ARRAY_BUFFER, 4*(CIRCLE_RAY_COUNT+2)*sizeof(GLfloat), colors.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

