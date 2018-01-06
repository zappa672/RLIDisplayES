#include "radarengine.h"

#include <QFile>
#include <QMatrix4x4>
#include <QDateTime>

static double const PI = acos(-1);

RadarEngine::RadarEngine(uint pel_count, uint pel_len, uint tex_radius, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {
  _has_data = false;
  _fbo = NULL;

  initializeOpenGLFunctions();

  _peleng_count = 0;
  _peleng_len = 0;

  glGenBuffers(ATTR_CNT, _vbo_ids);
  glGenBuffers(1, &_ind_vbo_id);
  vao.create();

  _program = new QOpenGLShaderProgram();
  _palette = new RadarPalette(context, this);

  initShader();
  initVAO();

  resizeData(pel_count, pel_len);
  resizeTexture(tex_radius);
}

RadarEngine::~RadarEngine() {
  delete _fbo;
  delete _program;

  glDeleteBuffers(ATTR_CNT, _vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
  vao.destroy();

  delete _palette;
}

void RadarEngine::onBrightnessChanged(int br) {
  _palette->setBrightness(br);
}


void RadarEngine::initVAO() {
  vao.bind();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POS]);
  //glVertexAttribPointer( _attr_locs[ATTR_POS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glVertexAttribPointer(_attr_locs[ATTR_POS], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_POS]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMP]);
  glVertexAttribPointer( _attr_locs[ATTR_AMP], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_AMP]);

  vao.release();
}


void RadarEngine::initShader() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/radar.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/radar.frag.glsl");
  _program->link();
  _program->bind();

  _unif_locs[UNIF_MVP] = _program->uniformLocation("mvp_matrix");
  _unif_locs[UNIF_TEX] = _program->uniformLocation("texture");
  _unif_locs[UNIF_THR] = _program->uniformLocation("threashold");
  _unif_locs[UNIF_PLN] = _program->uniformLocation("peleng_length");
  _unif_locs[UNIF_PCN] = _program->uniformLocation("peleng_count");

  _attr_locs[ATTR_POS] = _program->attributeLocation("position");
  _attr_locs[ATTR_AMP] = _program->attributeLocation("amplitude");

  _program->release();
}


void RadarEngine::resizeData(uint pel_count, uint pel_len) {
  if (_peleng_count == pel_count && _peleng_len == pel_len)
    return;

  _peleng_count = pel_count;
  _peleng_len = pel_len;

  fillCoordTable();
  clearData();
}


void RadarEngine::fillCoordTable() {
  _positions.clear();
  _draw_indices.clear();

  uint total = _peleng_count*_peleng_len;

  for (uint index = 0; index < _peleng_count; index++) {
    for (uint radius = 0; radius < _peleng_len; radius++) {
      uint curr_index = index*_peleng_len + radius;
      uint prev_index = ((index-1)*_peleng_len + radius + total) % total;

      _draw_indices.push_back(curr_index);
      _draw_indices.push_back(prev_index);
      _positions.push_back(curr_index);
    }

    GLuint last = _draw_indices[_draw_indices.size()-1];
    _draw_indices.push_back(last);
    _draw_indices.push_back((last+1)%total);
  }
}

void RadarEngine::resizeTexture(uint radius) {
  if (_fbo != NULL) {
    if (_fbo->size().width() == static_cast<int>(2*radius+1)) {
      return;
    }

    delete _fbo;
  }

  _radius = radius;

  QOpenGLFramebufferObjectFormat format;
  format.setAttachment(QOpenGLFramebufferObject::Depth);

  _fbo = new QOpenGLFramebufferObject(2*_radius+1, 2*_radius+1, format);

  clearTexture();
}


void RadarEngine::clearData() {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POS]);
  //glBufferData(GL_ARRAY_BUFFER, 2*_peleng_count*_peleng_len*sizeof(GLfloat), _positions.data(), GL_DYNAMIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, _peleng_count*_peleng_len*sizeof(GLfloat), _positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMP]);
  glBufferData(GL_ARRAY_BUFFER, _peleng_count*_peleng_len*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _draw_indices.size()*sizeof(GLuint), _draw_indices.data(), GL_STATIC_DRAW);

  _draw_circle       = false;
  _has_data          = false;
  _last_drawn_peleng = _peleng_count - 1;
  _last_added_peleng = _peleng_count - 1;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void RadarEngine::updateData(uint offset, uint count, GLfloat* amps) {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMP]);
  glBufferSubData(GL_ARRAY_BUFFER, offset*_peleng_len*sizeof(GLfloat), count*_peleng_len*sizeof(GLfloat), amps);

  // New last added peleng
  uint nlap = (offset + count - 1) % _peleng_count;

  // If we recieved full circle after last draw
  _draw_circle = (_last_added_peleng < _last_drawn_peleng && nlap >= _last_drawn_peleng) || count == _peleng_len;
  _last_added_peleng = nlap;

  if (!_has_data) {
    _draw_circle = false;
    _last_added_peleng = offset % _peleng_count;
    _has_data = true;
  }
}


void RadarEngine::clearTexture() {
  glDepthFunc(GL_ALWAYS);

  _fbo->bind();

  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glClearDepthf(0.f);
  glClear(GL_DEPTH_BUFFER_BIT);

  _fbo->release();
}


void RadarEngine::updateTexture() {
  if (!_has_data) {
    clearTexture();
    return;
  }

  // Calculate which pelengs we should draw
  // --------------------------------------
  if (_last_added_peleng == _last_drawn_peleng && !_draw_circle)
    return;

  uint first_peleng_to_draw = (_last_drawn_peleng + 1) % _peleng_count;
  uint last_peleng_to_draw = _last_added_peleng % _peleng_count;

  if (_draw_circle)
    first_peleng_to_draw = (_last_added_peleng + 1) % _peleng_count;
  // --------------------------------------


  // --------------------------------------
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);

  glViewport(0, 0, _fbo->width(), _fbo->height());

  _fbo->bind();  

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0, _fbo->width(), 0, _fbo->height(), -2, 2);

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate(_fbo->width() / 2.f, _fbo->height() / 2.f, 0.f);

  _program->bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _palette->texture());

  _program->setUniformValue(_unif_locs[UNIF_TEX], 0);
  _program->setUniformValue(_unif_locs[UNIF_MVP], projection*transform);
  _program->setUniformValue(_unif_locs[UNIF_THR], 64.f);

  _program->setUniformValue(_unif_locs[UNIF_PLN], static_cast<GLfloat>(_peleng_len));
  _program->setUniformValue(_unif_locs[UNIF_PCN], static_cast<GLfloat>(_peleng_count));

  if (first_peleng_to_draw <= last_peleng_to_draw) {
    drawPelengs(first_peleng_to_draw, last_peleng_to_draw);
  } else {
    drawPelengs(first_peleng_to_draw, _peleng_count - 1);
    drawPelengs(0, last_peleng_to_draw);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  _program->release();

  _fbo->release();
  // --------------------------------------

  _last_drawn_peleng = last_peleng_to_draw;
  _draw_circle = false;

  glFlush();
}

void RadarEngine::drawPelengs(uint first, uint last) {
  // Clear depth when the new cycle begins to avoid the previous circle data
  if (first == 0) {
    glClearDepthf(0.f);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  vao.bind();

  glDepthFunc(GL_GREATER);
  glDrawElements( GL_TRIANGLE_STRIP
                , (last-first+1)*(2*_peleng_len+2)
                , GL_UNSIGNED_INT
                , (const GLvoid*)(first*(2*_peleng_len+2) * sizeof(GLuint)));

  vao.release();
}
