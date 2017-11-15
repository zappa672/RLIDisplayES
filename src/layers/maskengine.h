#ifndef MASKENGINE_H
#define MASKENGINE_H

#define CIRCLE_RAY_COUNT 720
#define MARK_RAY_COUNT 36

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>


class MaskEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  explicit MaskEngine (const QSize& sz, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~MaskEngine ();

  void resize(const QSize& sz);

  inline GLuint textureId()   { return _fbo->texture(); }

private:
  void update();

  void initBuffers();
  void initShader();

  void initRectBuffers();
  void initHoleBuffers();


  QOpenGLFramebufferObject* _fbo;
  QOpenGLShaderProgram* _program;

  enum { ATTR_POS = 0, ATTR_CLR= 1, ATTR_CNT = 2 } ;
  enum { UNIF_MVP = 0, UNIF_CNT = 1 } ;

  GLuint _rect_vbo_ids[ATTR_CNT];
  GLuint _hole_vbo_ids[ATTR_CNT];

  GLuint _unif_locs[UNIF_CNT];
  GLuint _attr_locs[ATTR_CNT];
};
#endif // MASKENGINE_H
