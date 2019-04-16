#include "magnifierengine.h"
#include "../common/properties.h"

#include <vector>

MagnifierEngine::MagnifierEngine(const RLIMagnifierLayout& layout, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _prog = new QOpenGLShaderProgram();
  _fbo = nullptr;

  glGenBuffers(MAGN_ATTR_COUNT, _vbo_ids_border);
  glGenBuffers(MAGN_ATTR_COUNT, _vbo_ids_radar);

  initShaders();

  resize(layout);
}

MagnifierEngine::~MagnifierEngine() {
  delete _prog;
  delete _fbo;

  glDeleteBuffers(MAGN_ATTR_COUNT, _vbo_ids_border);
  glDeleteBuffers(MAGN_ATTR_COUNT, _vbo_ids_radar);
}

void MagnifierEngine::resize(const RLIMagnifierLayout& layout) {
  if (_fbo != nullptr)
    delete _fbo;

  _fbo = new QOpenGLFramebufferObject(layout.geometry.size());
  _geometry = layout.geometry;

  initBorderBuffers();
  initRadarBuffers();
}

void MagnifierEngine::update(int pel_len, int pel_cnt, int min_pel, int min_rad) {
  glViewport(0, 0, _fbo->width(), _fbo->height());

  _fbo->bind();

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _fbo->width(), _fbo->height(), 0.f, -1.f, 1.f);

  _prog->bind();

  _prog->setUniformValue(_unif_locs[MAGN_UNIF_MVP], projection);

  glUniform1i(_unif_locs[MAGN_UNIF_TEXTURE], 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _pal_tex_id);

  drawPelengs(pel_len, pel_cnt, min_pel, min_rad);
  drawBorder();

  glBindTexture(GL_TEXTURE_2D, 0);

  _prog->release();
  _fbo->release();
}

void MagnifierEngine::drawPelengs(int pel_len, int pel_cnt, int min_pel, int min_rad) {
  glUniform4f(_unif_locs[MAGN_UNIF_COLOR], 0.0f, 0.0f, 0.0f, 1.0f);
  glUniform1f(_unif_locs[MAGN_UNIF_THREASHOLD], 1.f);

  for (int i = 0; i < _fbo->width() - 2; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_radar[MAGN_ATTR_POSITION]);
    glVertexAttribPointer(_attr_locs[MAGN_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (2 * (_fbo->height()-2) * i * sizeof(GLfloat)));
    glEnableVertexAttribArray(_attr_locs[MAGN_ATTR_POSITION]);

    int amp_shift = ((min_pel + i) % pel_cnt) * pel_len + min_rad;

    glBindBuffer(GL_ARRAY_BUFFER, _amp_vbo_id);
    glVertexAttribPointer(_attr_locs[MAGN_ATTR_AMPLITUDE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (amp_shift * sizeof(GLfloat)));
    glEnableVertexAttribArray(_attr_locs[MAGN_ATTR_AMPLITUDE]);

    glDrawArrays(GL_POINTS, 0, (_fbo->height()-2));
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MagnifierEngine::drawBorder() {
  glUniform4f(_unif_locs[MAGN_UNIF_COLOR], 0.0f, 1.0f, 0.0f, 1.0f);
  glUniform1f(_unif_locs[MAGN_UNIF_THREASHOLD], 255.f);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_border[MAGN_ATTR_POSITION]);
  glVertexAttribPointer(_attr_locs[MAGN_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[MAGN_ATTR_POSITION]);

  glVertexAttrib1f(_attr_locs[MAGN_ATTR_AMPLITUDE], 0.f);
  glDisableVertexAttribArray(_attr_locs[MAGN_ATTR_AMPLITUDE]);

  glLineWidth(1.f);
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MagnifierEngine::initShaders() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "magn.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "magn.frag.glsl");
  _prog->link();
  _prog->bind();

  _attr_locs[MAGN_ATTR_POSITION]    = _prog->attributeLocation("a_position");
  _attr_locs[MAGN_ATTR_AMPLITUDE]   = _prog->attributeLocation("a_amplitude");

  _unif_locs[MAGN_UNIF_MVP]         = _prog->uniformLocation("mvp_matrix");
  _unif_locs[MAGN_UNIF_COLOR]       = _prog->uniformLocation("color");
  _unif_locs[MAGN_UNIF_TEXTURE]     = _prog->uniformLocation("texture");
  _unif_locs[MAGN_UNIF_THREASHOLD]  = _prog->uniformLocation("threashold");

  _prog->release();
}

void MagnifierEngine::initBorderBuffers() {
  GLfloat positions[] { 0.5f                 , 0.5f
                      , 0.5f                 , _fbo->height()-0.5f
                      , _fbo->width()-0.5f   , _fbo->height()-0.5f
                      , _fbo->width()-0.5f   , 0.5f                 };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_border[MAGN_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), positions, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MagnifierEngine::initRadarBuffers() {
  std::vector<GLfloat> positions;

  for (float i = 1.5; i < _fbo->width() - 1; i++) {
    for (float j = 1.5; j < _fbo->height() - 1; j++) {
      positions.push_back(i);
      positions.push_back(j);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_radar[MAGN_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
