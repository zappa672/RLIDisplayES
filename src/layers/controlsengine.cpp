#include "controlsengine.h"
#include "../common/properties.h"

#include <vector>

ControlsEngine::ControlsEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _vn_p = 0.f;
  _vn_cu = 0.f;
  _vd = 0.f;

  _showCircles = false;
  _showParallelLines = false;

  _prog = new QOpenGLShaderProgram();

  glGenBuffers(CTRL_ATTR_COUNT, _vbo_ids_cursor);
  glGenBuffers(CTRL_ATTR_COUNT, _vbo_ids_circle);
  glGenBuffers(CTRL_ATTR_COUNT, _vbo_ids_ray);

  initShaders();

  initCursorBuffers();
  initCircleBuffers();
}

ControlsEngine::~ControlsEngine() {
  glDeleteBuffers(CTRL_ATTR_COUNT, _vbo_ids_cursor);
  glDeleteBuffers(CTRL_ATTR_COUNT, _vbo_ids_circle);
  glDeleteBuffers(CTRL_ATTR_COUNT, _vbo_ids_ray);

  delete _prog;
}

void ControlsEngine::draw(const QMatrix4x4& mvp_mat) {
  _prog->bind();
  _prog->setUniformValue(_unif_locs[CTRL_UNIF_MVP], mvp_mat);

  drawCursor(QColor(255, 0, 255, 255));

  // Визир дальности
  drawCircleSegment(QColor(255, 255, 255, 255),  _vd);

  if (_showCircles) {
    drawCircleSegment(QColor(203, 67, 69, 255),  80.f);
    drawCircleSegment(QColor(203, 67, 69, 255), 160.f);
    drawCircleSegment(QColor(203, 67, 69, 255), 240.f);
    drawCircleSegment(QColor(203, 67, 69, 255), 320.f);
    drawCircleSegment(QColor(203, 67, 69, 255), 400.f);
    drawCircleSegment(QColor(203, 67, 69, 255), 480.f);
  }

  // Визиры направления
  drawRaySegment(QColor(255, 192, 26, 255), _vn_cu);
  drawRaySegment(QColor(255, 192, 26, 255), _vn_p);

  // Область захвата
  drawRaySegment   (QColor(255, 255, 0, 255),  280.f,   48.f,  112.f);
  drawRaySegment   (QColor(255, 255, 0, 255),  340.f,   48.f,  112.f);
  drawCircleSegment(QColor(255, 255, 0, 255),   48.f,  280.f,  340.f);
  drawCircleSegment(QColor(255, 255, 0, 255),  112.f,  280.f,  340.f);

  // Лупа
  drawRaySegment   (QColor(0, 0, 255, 255), (90.f / 4096.f) * 360.f        ,  96.f                   ,  96.f + 224.f);
  drawRaySegment   (QColor(0, 0, 255, 255), (90.f + 224.f / 4096.f) * 360.f,  96.f                   ,  96.f + 224.f);
  drawCircleSegment(QColor(0, 0, 255, 255),  96.f                          ,  (90.f / 4096.f) * 360.f,  (90.f + 224.f / 4096.f) * 360.f);
  drawCircleSegment(QColor(0, 0, 255, 255),  96.f + 224.f                  ,  (90.f / 4096.f) * 360.f,  (90.f + 224.f / 4096.f) * 360.f);


  if (_showParallelLines) {
    drawRaySegment(QColor(255, 255, 255, 255), _vn_p, -2048.f, 2048.f,  _vd);
    drawRaySegment(QColor(255, 255, 255, 255), _vn_p, -2048.f, 2048.f, -_vd);
  }

  _prog->release();
}

void ControlsEngine::initShaders() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "ctrl.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "ctrl.frag.glsl");

  _prog->link();
  _prog->bind();

  _attr_locs[CTRL_ATTR_ANGLE] = _prog->attributeLocation("angle");
  _attr_locs[CTRL_ATTR_RADIUS] = _prog->attributeLocation("radius");

  _unif_locs[CTRL_UNIF_MVP] = _prog->uniformLocation("mvp_matrix");
  _unif_locs[CTRL_UNIF_COLOR] = _prog->uniformLocation("color");
  _unif_locs[CTRL_UNIF_SHIFT] = _prog->uniformLocation("shift");

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



void ControlsEngine::drawCursor(const QColor& col) {
  glUniform1f(_unif_locs[CTRL_UNIF_SHIFT], 0.0);
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[CTRL_ATTR_ANGLE]);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  glVertexAttrib1f(_attr_locs[CTRL_ATTR_RADIUS], 5.f);
  glDisableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  glLineWidth(2.f);
  glDrawArrays(GL_LINES, 0, 4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::drawCircleSegment(const QColor& col, GLfloat radius, GLfloat min_angle, GLfloat max_angle) {
  glUniform1f(_unif_locs[CTRL_UNIF_SHIFT], 0.0);
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glLineWidth(2.f);

  glVertexAttrib1f(_attr_locs[CTRL_ATTR_RADIUS], radius);
  glDisableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  int frst_element = (int((min_angle / 360) * 720) + 720) % 720;
  int last_element = (int((max_angle / 360) * 720) + 720) % 720;

  if (last_element == 0)
    last_element = 720;

  if (last_element > frst_element) {
      glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[CTRL_ATTR_ANGLE]);
      glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (frst_element * sizeof(GLfloat)));
      glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

      glDrawArrays(GL_LINE_STRIP, 0, last_element - frst_element);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::drawRaySegment(const QColor& col, GLfloat angle, GLfloat min_radius, GLfloat max_radius, GLfloat shift) {
  glUniform1f(_unif_locs[CTRL_UNIF_SHIFT], shift);
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  GLfloat angs[] { angle, angle };
  GLfloat rads[] { min_radius, max_radius };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[CTRL_ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat), angs, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[CTRL_ATTR_RADIUS]);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat), rads, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_RADIUS], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  glLineWidth(2.f);
  glDrawArrays(GL_LINES, 0, 2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
