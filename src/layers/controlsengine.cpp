#include "controlsengine.h"

#include <vector>

ControlsEngine::ControlsEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _prog = new QOpenGLShaderProgram();

  glGenBuffers(CTRL_ATTR_COUNT, _vbo_ids_cursor);
  glGenBuffers(CTRL_ATTR_COUNT, _vbo_ids_circle);
  glGenBuffers(CTRL_ATTR_COUNT, _vbo_ids_ray);

  initShaders();

  initCursorBuffers();
  initCircleBuffers();
  initRayBuffers();
}

ControlsEngine::~ControlsEngine() {
  glDeleteBuffers(CTRL_ATTR_COUNT, _vbo_ids_cursor);
  glDeleteBuffers(CTRL_ATTR_COUNT, _vbo_ids_circle);
  glDeleteBuffers(CTRL_ATTR_COUNT, _vbo_ids_ray
                  );

  delete _prog;
}

void ControlsEngine::draw(const QMatrix4x4& mvp_mat) {
  _prog->bind();

  drawCursor(mvp_mat);
  drawCircle(mvp_mat, 64.f);
  drawRay(mvp_mat, 45.f);
  drawRay(mvp_mat, 235.f);

  _prog->release();
}

void ControlsEngine::initShaders() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ctrl.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ctrl.frag.glsl");

  _prog->link();
  _prog->bind();

  _attr_locs[CTRL_ATTR_ANGLE] = _prog->attributeLocation("angle");
  _attr_locs[CTRL_ATTR_RADIUS] = _prog->attributeLocation("radius");

  _unif_locs[CTRL_UNIF_MVP] = _prog->uniformLocation("mvp_matrix");
  _unif_locs[CTRL_UNIF_COLOR] = _prog->uniformLocation("color");

  _prog->release();
}

void ControlsEngine::initCursorBuffers() {
  GLfloat angs[] { 0.f, 180.f, 90.f, 270.f };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[CTRL_ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat), angs, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::initCircleBuffers() {
  GLfloat angs[720];

  for (int i = 0; i < 720; i++)
    angs[i] = (i / 719.f) * 360.f;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[CTRL_ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, 720*sizeof(GLfloat), angs, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::initRayBuffers() {
  GLfloat rads[] { 0.f, 4096.f };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[CTRL_ATTR_RADIUS]);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat), rads, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void ControlsEngine::drawCursor(const QMatrix4x4& mvp_mat) {
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], 1.0, 1.0, 0.0, 1.0);
  _prog->setUniformValue(_unif_locs[CTRL_UNIF_MVP], mvp_mat);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[CTRL_ATTR_ANGLE]);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  glVertexAttrib1f(_attr_locs[CTRL_ATTR_RADIUS], 5.f);
  glDisableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  glLineWidth(2.f);
  glDrawArrays(GL_LINES, 0, 4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::drawCircle(const QMatrix4x4& mvp_mat, float radius) {
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], 1.0, 0.0, 1.0, 1.0);
  _prog->setUniformValue(_unif_locs[CTRL_UNIF_MVP], mvp_mat);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[CTRL_ATTR_ANGLE]);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  glVertexAttrib1f(_attr_locs[CTRL_ATTR_RADIUS], radius);
  glDisableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  glLineWidth(1.f);

  glDrawArrays(GL_LINE_LOOP, 0, 720);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::drawRay(const QMatrix4x4& mvp_mat, float angle) {
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], 0.0, 1.0, 1.0, 1.0);
  _prog->setUniformValue(_unif_locs[CTRL_UNIF_MVP], mvp_mat);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[CTRL_ATTR_RADIUS]);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_RADIUS], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  glVertexAttrib1f(_attr_locs[CTRL_ATTR_ANGLE], angle);
  glDisableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  glLineWidth(1.f);

  glDrawArrays(GL_LINES, 0, 2);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
