#include "maskengine.h"

#include "../common/properties.h"
#include "../common/rliconfig.h"

static double const PI = acos(-1);

MaskEngine::MaskEngine(const QSize& sz, const RLICircleInfo& params, InfoFonts* fonts, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {

  initializeOpenGLFunctions();

  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_mark);
  glGenBuffers(1, &_ind_vbo_id_text);
  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_text);
  glGenBuffers(MASK_ATTR_COUNT, vbo_ids_hole);

  _fonts = fonts;
  _fbo = nullptr;
  _program = new QOpenGLShaderProgram();

  _angle_shift = 0;
  _text_point_count = 0;
  _hole_point_count = 362;

  initShader();
  resize(sz, params);
}

MaskEngine::~MaskEngine() {
  glDeleteBuffers(MASK_ATTR_COUNT, vbo_ids_mark);
  glDeleteBuffers(MASK_ATTR_COUNT, vbo_ids_text);
  glDeleteBuffers(MASK_ATTR_COUNT, vbo_ids_hole);

  delete _fbo;
  delete _program;
}

void MaskEngine::resize(const QSize& sz, const RLICircleInfo& params) {
  if (_fbo != nullptr && sz == _fbo->size())
     return;

  _size = sz;
  _font = params.font;
  _hole_radius = params.radius;
  _hole_center = params.center;
  _cursor_pos = _hole_center;

  delete _fbo;
  _fbo = new QOpenGLFramebufferObject(sz);

  initBuffers();
  update();
}


void MaskEngine::update() {
  _fbo->bind();

  glViewport(0, 0, _size.width(), _size.height());

  glClearColor(0.12, 0.13, 0.10, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _size.width(), 0.f, _size.height(), -1.f, 1.f);

  // Start mark shader drawing
  _program->bind();

  // Set uniforms
  // ---------------------------------------------------------------------
  _program->setUniformValue(_unif_locs[MASK_UNIF_MVP], projection);
  glUniform1f(_unif_locs[MASK_UNIF_ANGLE_SHIFT], _angle_shift);
  glUniform1f(_unif_locs[MASK_UNIF_CIRCLE_RADIUS], _hole_radius);
  glUniform2f(_unif_locs[MASK_UNIF_CIRCLE_POS], _hole_center.x(), _hole_center.y());
  glUniform2f(_unif_locs[MASK_UNIF_CURSOR_POS], _cursor_pos.x(), _cursor_pos.y());
  glUniform4f(_unif_locs[MASK_UNIF_COLOR], 0.f, 1.f, 0.f, 1.f);
  // ---------------------------------------------------------------------

  // Draw line marks
  // ---------------------------------------------------------------------
  bindBuffers(vbo_ids_mark);
  glLineWidth(1);
  glDrawArrays(GL_LINES, 0, 2*360);
  // ---------------------------------------------------------------------

  // Draw text mark
  // ---------------------------------------------------------------------
  QSizeF font_size = _fonts->getFontSize(_font);
  GLuint tex_id = _fonts->getTexture(_font)->textureId();

  bindBuffers(vbo_ids_text);

  glUniform2f(_unif_locs[MASK_UNIF_FONT_SIZE], font_size.width(), font_size.height());
  glUniform1f(_unif_locs[MASK_UNIF_GLYPH_TEX], 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text);
  glDrawElements(GL_TRIANGLES, 3*(_text_point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  // ---------------------------------------------------------------------

  glDisable(GL_BLEND);

  // Draw hole
  // ---------------------------------------------------------------------
  glUniform4f(_unif_locs[MASK_UNIF_COLOR], 1.f, 1.f, 1.f, 0.f);
  glUniform2f(_unif_locs[MASK_UNIF_CURSOR_POS], _hole_center.x(), _hole_center.y());

  bindBuffers(vbo_ids_hole);

  glDrawArrays(GL_TRIANGLE_FAN, 0, _hole_point_count+2);
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

  _attr_locs[MASK_ATTR_ANGLE]         = _program->attributeLocation("angle");
  _attr_locs[MASK_ATTR_CHAR_VAL]      = _program->attributeLocation("char_val");
  _attr_locs[MASK_ATTR_ORDER]         = _program->attributeLocation("order");
  _attr_locs[MASK_ATTR_SHIFT]         = _program->attributeLocation("shift");

  _program->release();
}




void MaskEngine::initBuffers() {
  initLineBuffers();
  initTextBuffers();
  initHoleBuffers();
  initRectBuffers();
}

void MaskEngine::setBuffers(GLuint* vbo_ids, int count, GLfloat* angles, GLfloat* chars, GLfloat* orders, GLfloat* shifts) {
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
  glVertexAttribPointer(_attr_locs[MASK_ATTR_ANGLE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_ANGLE]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_CHAR_VAL]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_CHAR_VAL]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_ORDER]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[MASK_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MASK_ATTR_SHIFT]);
  glVertexAttribPointer(_attr_locs[MASK_ATTR_SHIFT], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
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

  for (int a = 0; a < _hole_point_count; a++) {
    angle.push_back((static_cast<float>(a) / _hole_point_count) * 360);
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
  QVector<GLfloat> angles;
  QVector<GLfloat> chars;
  QVector<GLfloat> orders;
  QVector<GLfloat> shifts;

  _text_point_count = 0;

  for (int i = 0; i < 360; i += 10) {
    QByteArray tm = QString::number(i).toLatin1();

    for (int l = 0; l < tm.size(); l++) {
      for (int k = 0; k < 4; k++) {
        angles.push_back(i);
        chars.push_back(tm[l]);
        orders.push_back(k);
        shifts.push_back(l);
      }
    }
  }

  _text_point_count = orders.size();
  setBuffers(vbo_ids_text, _text_point_count, angles.data(), chars.data(), orders.data(), shifts.data());


  std::vector<GLuint> draw_indices;

  for (int i = 0; i < _text_point_count; i += 4) {
    draw_indices.push_back(i);
    draw_indices.push_back(i+1);
    draw_indices.push_back(i+2);
    draw_indices.push_back(i);
    draw_indices.push_back(i+2);
    draw_indices.push_back(i+3);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw_indices.size()*sizeof(GLuint), draw_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
