#include "magnifierengine.h"

#include <vector>

MagnifierEngine::MagnifierEngine(const QMap<QString, QString>& params, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _visible = false;
  _prog = new QOpenGLShaderProgram();
  _fbo = nullptr;

  glGenBuffers(MAGN_ATTR_COUNT, _vbo_ids);
  initShaders();

  resize(params);
}

MagnifierEngine::~MagnifierEngine() {
  delete _prog;
  delete _fbo;
  glDeleteBuffers(MAGN_ATTR_COUNT, _vbo_ids);
}

void MagnifierEngine::resize(const QMap<QString, QString>& params) {
  _size = QSize(params["width"].toInt(), params["height"].toInt());
  _position = QPoint(params["x"].toInt(), params["y"].toInt());

  if (_fbo != nullptr)
    delete _fbo;

  _fbo = new QOpenGLFramebufferObject(_size);
  initBorderBuffers();
}

void MagnifierEngine::update() {
  if (!_visible)
    return;

  glViewport(0, 0, _size.width(), _size.height());

  _fbo->bind();

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _size.width(), 0.f, _size.height(), -1.f, 1.f);

  _prog->bind();

  _prog->setUniformValue(_unif_locs[MAGN_UNIF_MVP], projection);
  glUniform4f(_unif_locs[MAGN_UNIF_COLOR], 1.0f, 0.0f, 0.0f, 1.0f);
  glUniform1i(_unif_locs[MAGN_UNIF_TEXTURE], 0);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[MAGN_ATTR_POSITION]);
  glVertexAttribPointer(_attr_locs[MAGN_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[MAGN_ATTR_POSITION]);

  glVertexAttrib1f(_attr_locs[MAGN_ATTR_AMPLITUDE], -1);
  glDisableVertexAttribArray(_attr_locs[MAGN_ATTR_AMPLITUDE]);

  glDrawArrays(GL_LINE_LOOP, 0, 4);

  _prog->release();
  _fbo->release();
}

void MagnifierEngine::initShaders() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/magn.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/magn.frag.glsl");
  _prog->link();
  _prog->bind();

  _attr_locs[MAGN_ATTR_POSITION]  = _prog->attributeLocation("a_position");
  _attr_locs[MAGN_ATTR_AMPLITUDE] = _prog->attributeLocation("a_amplitude");

  _unif_locs[MAGN_UNIF_MVP]       = _prog->uniformLocation("mvp_matrix");
  _unif_locs[MAGN_UNIF_COLOR]     = _prog->uniformLocation("color");
  _unif_locs[MAGN_UNIF_TEXTURE]   = _prog->uniformLocation("texture");

  _prog->release();
}

void MagnifierEngine::initBorderBuffers() {
  QSizeF s(_size);
  GLfloat positions[] { 0.f, 0.f
                      , 0.f, static_cast<GLfloat>(_size.height())
                      , static_cast<GLfloat>(s.width()), static_cast<GLfloat>(s.height())
                      , static_cast<GLfloat>(s.width()), 0.f };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[MAGN_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), positions, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
