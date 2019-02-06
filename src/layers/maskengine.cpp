#include "maskengine.h"

#include "../common/properties.h"
#include "../common/rlilayout.h"

#include <qmath.h>

static double const PI = acos(-1);

MaskEngine::MaskEngine(const QSize& sz, const RLICircleLayout& layout, InfoFonts* fonts, QOpenGLContext* context, const RLIState& _rli_state, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {

  initializeOpenGLFunctions();

  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_mark);
  glGenBuffers(1, &_ind_vbo_id_text);
  glGenBuffers(1, &_ind_vbo_id_text2);
  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_text);
  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_text2);
  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_hole);

  _fonts = fonts;
  _fbo = nullptr;
  _program = new QOpenGLShaderProgram();

  _text_point_count = 0;
  _hole_point_count = 362;

  initShader();
  resize(sz, layout, _rli_state);
}

MaskEngine::~MaskEngine() {
  glDeleteBuffers(MASK_ATTR_COUNT, vbo_ids_mark);
  glDeleteBuffers(MASK_ATTR_COUNT, vbo_ids_text);
  glDeleteBuffers(MASK_ATTR_COUNT, vbo_ids_hole);

  delete _fbo;
  delete _program;
}

void MaskEngine::resize(const QSize& sz, const RLICircleLayout& layout, const RLIState& _rli_state) {
  _angle_shift = -360; // enforce update

  delete _fbo;
  _fbo = new QOpenGLFramebufferObject(sz);

  initBuffers();
  update(_rli_state, layout, true);
}


void MaskEngine::update(const RLIState& rli_state, const RLICircleLayout& layout, bool forced) {
  if ( !forced
    && fabs(rli_state.north_shift - _angle_shift) < 1.0
    && QVector2D(_center_shift - rli_state.center_shift).length() < 1.f
    && _orient == rli_state.orientation )
    return;

  _angle_shift = rli_state.north_shift;
  _center_shift = rli_state.center_shift;
  _orient = rli_state.orientation;

  _fbo->bind();

  glViewport(0, 0, _fbo->width(), _fbo->height());

  glClearColor(0.12f, 0.13f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _fbo->width(), 0.f, _fbo->height(), -1.f, 1.f);

  // Start mark shader drawing
  _program->bind();

  // Set uniforms
  // ---------------------------------------------------------------------
  _program->setUniformValue(_unif_locs[MASK_UNIF_MVP], projection);
  glUniform1f(_unif_locs[MASK_UNIF_ANGLE_SHIFT], static_cast<float>(_angle_shift));
  glUniform1f(_unif_locs[MASK_UNIF_CIRCLE_RADIUS], layout.radius);
  glUniform2f(_unif_locs[MASK_UNIF_CIRCLE_POS], layout.center.x(), layout.center.y());
  glUniform4f(_unif_locs[MASK_UNIF_COLOR], 0.f, 1.f, 0.f, 1.f);
  glUniform2f( _unif_locs[MASK_UNIF_CURSOR_POS], static_cast<float>(layout.center.x() + _center_shift.x())
                                               , static_cast<float>(layout.center.y() + _center_shift.y()));
  // ---------------------------------------------------------------------

  // Draw line marks
  // ---------------------------------------------------------------------
  bindBuffers(vbo_ids_mark);
  glLineWidth(1);
  glDrawArrays(GL_LINES, 0, 2*360);
  // ---------------------------------------------------------------------

  // Draw text mark
  // ---------------------------------------------------------------------
  QSize font_size = _fonts->getFontSize(layout.font);
  GLuint tex_id = _fonts->getTexture(layout.font)->textureId();

  glUniform2f(_unif_locs[MASK_UNIF_FONT_SIZE], font_size.width(), font_size.height());
  glUniform1f(_unif_locs[MASK_UNIF_GLYPH_TEX], 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  if (rli_state.orientation == RLIOrientation::RLIORIENT_NORTH) {
    bindBuffers(vbo_ids_text);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text);
    glDrawElements(GL_TRIANGLES, 3*(_text_point_count/2), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0 * sizeof(GLuint)));
  } else {
    bindBuffers(vbo_ids_text2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text2);
    glDrawElements(GL_TRIANGLES, 3*(_text_point_count2/2), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0 * sizeof(GLuint)));
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  // ---------------------------------------------------------------------

  glDisable(GL_BLEND);

  // Draw hole
  // ---------------------------------------------------------------------
  glUniform4f(_unif_locs[MASK_UNIF_COLOR], 1.f, 1.f, 1.f, 0.f);
  bindBuffers(vbo_ids_hole);
  glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<int>(_hole_point_count+2));
  // ---------------------------------------------------------------------

  _program->release();
  _fbo->release();
}



void MaskEngine::initShader() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "mask.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "mask.frag.glsl");
  _program->link();
  _program->bind();

  _unif_locs[MASK_UNIF_MVP]           = _program->uniformLocation("mvp_matrix");
  _unif_locs[MASK_UNIF_ANGLE_SHIFT]   = _program->uniformLocation("angle_shift");
  _unif_locs[MASK_UNIF_CIRCLE_RADIUS] = _program->uniformLocation("circle_radius");
  _unif_locs[MASK_UNIF_CIRCLE_POS]    = _program->uniformLocation("circle_pos");
  _unif_locs[MASK_UNIF_CURSOR_POS]    = _program->uniformLocation("cursor_pos");
  _unif_locs[MASK_UNIF_COLOR]         = _program->uniformLocation("color");
  _unif_locs[MASK_UNIF_FONT_SIZE]     = _program->uniformLocation("font_size");
  _unif_locs[MASK_UNIF_GLYPH_TEX]     = _program->uniformLocation("glyph_tex");

  _attr_locs[MASK_ATTR_ANGLE]         = static_cast<GLuint>(_program->attributeLocation("angle"));
  _attr_locs[MASK_ATTR_CHAR_VAL]      = static_cast<GLuint>(_program->attributeLocation("char_val"));
  _attr_locs[MASK_ATTR_ORDER]         = static_cast<GLuint>(_program->attributeLocation("order"));
  _attr_locs[MASK_ATTR_SHIFT]         = static_cast<GLuint>(_program->attributeLocation("shift"));

  _program->release();
}




void MaskEngine::initBuffers() {
  initLineBuffers();
  initTextBuffers();
  initHoleBuffers();
  initRectBuffers();
}

void MaskEngine::setBuffers(GLuint* vbo_ids, uint count, GLfloat* angles, GLfloat* chars, GLfloat* orders, GLfloat* shifts) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), angles, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), chars, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), orders, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_SHIFT]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), shifts, GL_DYNAMIC_DRAW);
}

void MaskEngine::bindBuffers(GLuint* vbo_ids) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_ANGLE]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_ANGLE]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_CHAR_VAL]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_CHAR_VAL]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_ORDER]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_SHIFT]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_SHIFT], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_SHIFT]);
}



void MaskEngine::initRectBuffers() {

}

void MaskEngine::initHoleBuffers() {
  QVector<GLfloat> angle;
  QVector<GLfloat> chars, order, shift;

  angle.push_back(0);
  order.push_back(0);
  chars.push_back(0);
  shift.push_back(0);

  for (uint a = 0; a < _hole_point_count; a++) {
    angle.push_back((a * 360.f) / _hole_point_count);
    order.push_back(1);
    chars.push_back(0);
    shift.push_back(0);
  }

  angle.push_back(angle.at(1));
  order.push_back(1);
  chars.push_back(0);
  shift.push_back(0);

  setBuffers(vbo_ids_hole, _hole_point_count+2, angle.data(), chars.data(), order.data(), shift.data());
}

void MaskEngine::initLineBuffers() {
  GLfloat angle[2*360];
  GLfloat chars[2*360];
  GLfloat order[2*360];
  GLfloat shift[2*360];

  for (int a = 0; a < 360; a++) {
    angle[2*a+0] = angle[2*a+1] = a;
    order[2*a+0] = 0;
    order[2*a+1] = 1;
    chars[2*a+0] = chars[2*a+1] = 0;

    char s = 2;
    if (a%5  == 0) s =  4;
    if (a%10 == 0) s =  7;
    if (a%30 == 0) s = 12;

    shift[2*a+0] = shift[2*a+1] = s;
  }

  setBuffers(vbo_ids_mark, 2*360, angle, chars, order, shift);
}

void MaskEngine::initTextBuffers() {
  QVector<GLfloat> angles[2];
  QVector<GLfloat> chars[2];
  QVector<GLfloat> orders[2];
  QVector<GLfloat> shifts[2];

  _text_point_count = 0;

  for (int i = 0; i < 360; i += 10) {
    QByteArray tm0 = QString::number(i).toLocal8Bit();
    QByteArray tm1 = QString::number(i > 180 ? 360-i : i).toLocal8Bit();

    for (int l = 0; l < tm0.size(); l++) {
      for (int k = 0; k < 4; k++) {
        angles[0].push_back(i);
        chars[0].push_back(tm0[l]);
        orders[0].push_back(k);
        shifts[0].push_back(l);
      }
    }

    for (int l = 0; l < tm1.size(); l++) {
      for (int k = 0; k < 4; k++) {
        angles[1].push_back(i);
        chars[1].push_back(tm1[l]);
        orders[1].push_back(k);
        shifts[1].push_back(l);
      }
    }
  }

  _text_point_count = static_cast<uint>(orders[0].size());
  setBuffers(vbo_ids_text, _text_point_count, angles[0].data(), chars[0].data(), orders[0].data(), shifts[0].data());
  _text_point_count2 = static_cast<uint>(orders[1].size());
  setBuffers(vbo_ids_text2, _text_point_count2, angles[1].data(), chars[1].data(), orders[1].data(), shifts[1].data());


  std::vector<GLuint> draw_indices[2];

  for (uint i = 0; i < _text_point_count; i += 4) {
    draw_indices[0].push_back(i);
    draw_indices[0].push_back(i+1);
    draw_indices[0].push_back(i+2);
    draw_indices[0].push_back(i);
    draw_indices[0].push_back(i+2);
    draw_indices[0].push_back(i+3);
  }

  for (uint i = 0; i < _text_point_count2; i += 4) {
    draw_indices[1].push_back(i);
    draw_indices[1].push_back(i+1);
    draw_indices[1].push_back(i+2);
    draw_indices[1].push_back(i);
    draw_indices[1].push_back(i+2);
    draw_indices[1].push_back(i+3);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*_text_point_count*sizeof(GLuint), draw_indices[0].data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text2);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*_text_point_count2*sizeof(GLuint), draw_indices[0].data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
