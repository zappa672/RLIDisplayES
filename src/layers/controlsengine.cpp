#include "controlsengine.h"
#include "../common/properties.h"

#include <vector>
#include "../common/rlimath.h"

ControlsEngine::ControlsEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

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

void ControlsEngine::draw(const QMatrix4x4& mvp_mat, const RLIState& state) {
  _prog->bind();

  QMatrix4x4 transform;
  transform.setToIdentity();

  if (state.state == RLIWidgetState::RLISTATE_ROUTE_EDITION) {
    QPointF tr = RLIMath::coords_to_pos( state.ship_position, state.visir_center_pos, QPoint(0,0), state.chart_scale);
    transform.translate( static_cast<GLfloat>(tr.x()), static_cast<GLfloat>(tr.y()), 0.f);
  }

  _prog->setUniformValue(_unif_locs[CTRL_UNIF_MVP], mvp_mat*transform);


  // Визир дальности
  drawCircleSegment(RLI_CNTR_COLOR_VD,  static_cast<float>(state.vd));

  // Distance rings
  if (state.show_circles) {
    for (float rad = 80; rad < 560; rad += 80)
      drawCircleSegment(RLI_CNTR_COLOR_CIRCLES,  rad);
  }
  // ----------------------

  // Direction rays
  drawRaySegment(RLI_CNTR_COLOR_VN_CU, static_cast<float>(state.vn_cu));
  drawRaySegment(RLI_CNTR_COLOR_VN_P , static_cast<float>(state.vn_p));



  // ----------------------

  // Parallel lines
  if (state.show_parallel) {
    drawRaySegment(RLI_CNTR_COLOR_PAR_LINES, static_cast<float>(state.vn_p), -2048.f, 2048.f,  static_cast<float>(state.vd));
    drawRaySegment(RLI_CNTR_COLOR_PAR_LINES, static_cast<float>(state.vn_p), -2048.f, 2048.f, -static_cast<float>(state.vd));
  }
  // ----------------------

  // Capture zone
  drawRaySegment   (RLI_CNTR_COLOR_CAPT_AREA,  280.f,   48.f,  112.f);
  drawRaySegment   (RLI_CNTR_COLOR_CAPT_AREA,  340.f,   48.f,  112.f);
  drawCircleSegment(RLI_CNTR_COLOR_CAPT_AREA,   48.f,  280.f,  340.f);
  drawCircleSegment(RLI_CNTR_COLOR_CAPT_AREA,  112.f,  280.f,  340.f);
  // ----------------------

  // Magnifier zone
  float magn_min_angle = (state.magn_min_peleng / 4096.f) * 360.f + state.north_shift;
  float magn_max_angle = ((state.magn_min_peleng + state.magn_width) / 4096.f) * 360.f + state.north_shift;

  drawRaySegment   (RLI_CNTR_COLOR_MAGNIFIER, magn_min_angle,  state.magn_min_rad,  state.magn_min_rad + state.magn_height);
  drawRaySegment   (RLI_CNTR_COLOR_MAGNIFIER, magn_max_angle,  state.magn_min_rad,  state.magn_min_rad + state.magn_height);
  drawCircleSegment(RLI_CNTR_COLOR_MAGNIFIER, state.magn_min_rad  ,  magn_min_angle,  magn_max_angle);
  drawCircleSegment(RLI_CNTR_COLOR_MAGNIFIER, state.magn_min_rad + state.magn_height ,  magn_min_angle,  magn_max_angle);
  // ----------------------

  // Cursor
  QPointF cusor_shift = state.cursor_pos - state.center_shift;
  transform.setToIdentity();
  transform.translate(cusor_shift.x(), cusor_shift.y() , 0.f);
  _prog->setUniformValue(_unif_locs[CTRL_UNIF_MVP], mvp_mat*transform);

  drawCursor(RLI_CNTR_COLOR_CURSOR);
  // ----------------------


  _prog->release();
}



void ControlsEngine::initShaders() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "ctrl.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "ctrl.frag.glsl");

  _prog->link();
  _prog->bind();

  _attr_locs[CTRL_ATTR_ANGLE]       = _prog->attributeLocation("angle");
  _attr_locs[CTRL_ATTR_RADIUS]      = _prog->attributeLocation("radius");

  _unif_locs[CTRL_UNIF_MVP]         = _prog->uniformLocation("mvp_matrix");
  _unif_locs[CTRL_UNIF_COLOR]       = _prog->uniformLocation("color");
  _unif_locs[CTRL_UNIF_SHIFT]       = _prog->uniformLocation("shift");

  _prog->release();
}

void ControlsEngine::initCursorBuffers() {
  GLfloat angs[] { 0.f, 180.f, 90.f, 270.f };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[CTRL_ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat), angs, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::initCircleBuffers() {
  std::vector<GLfloat> angs;

  for (size_t i = 0; i < RLI_CNTR_CIRCLE_RESOLUTION; i++)
    angs.push_back( (static_cast<float>(i) / RLI_CNTR_CIRCLE_RESOLUTION) * 360.f );
  angs.push_back(0);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[CTRL_ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, angs.size()*sizeof(GLfloat), angs.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void ControlsEngine::drawCursor(const QColor& col) {
  glUniform1f(_unif_locs[CTRL_UNIF_SHIFT], 0.0);
  glUniform4f(_unif_locs[CTRL_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[CTRL_ATTR_ANGLE]);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
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

  glPointSize(1.f);

  glVertexAttrib1f(_attr_locs[CTRL_ATTR_RADIUS], radius);
  glDisableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  int frst_element = (int((min_angle / 360) * RLI_CNTR_CIRCLE_RESOLUTION) + RLI_CNTR_CIRCLE_RESOLUTION) % RLI_CNTR_CIRCLE_RESOLUTION;
  int last_element = (int((max_angle / 360) * RLI_CNTR_CIRCLE_RESOLUTION) + RLI_CNTR_CIRCLE_RESOLUTION) % RLI_CNTR_CIRCLE_RESOLUTION;

  if (last_element == 0)
    last_element = RLI_CNTR_CIRCLE_RESOLUTION;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[CTRL_ATTR_ANGLE]);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*)(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  if (last_element > frst_element) {
    glDrawArrays(GL_POINTS, frst_element, last_element - frst_element);
  } else {
    glDrawArrays(GL_POINTS, frst_element, RLI_CNTR_CIRCLE_RESOLUTION - frst_element);
    glDrawArrays(GL_POINTS, 0, last_element);
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
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_ANGLE]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[CTRL_ATTR_RADIUS]);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat), rads, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[CTRL_ATTR_RADIUS], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[CTRL_ATTR_RADIUS]);

  glLineWidth(1.f);
  glDrawArrays(GL_LINES, 0, 2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
