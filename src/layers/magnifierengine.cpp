#include "magnifierengine.h"

MagnifierEngine::MagnifierEngine(const QMap<QString, QString>& params, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _visible = false;
  _prog = new QOpenGLShaderProgram();
  _fbo = nullptr;

  resize(params);

  glGenBuffers(MAGN_ATTR_COUNT, _vbo_ids);
  initShaders();
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
}

void MagnifierEngine::update() {
  if (!_visible)
    return;

  glViewport(0, 0, _size.width(), _size.height());

  _fbo->bind();

  glClearColor(0.856f, 0.494f, 0.543f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _size.width(), 0.f, _size.height(), -1.f, 1.f);

  //_prog->bind();



  //_prog->release();
  _fbo->release();
}

void MagnifierEngine::initShaders() {

}
