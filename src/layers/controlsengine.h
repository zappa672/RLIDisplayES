#ifndef CONTROLSENGINE_H
#define CONTROLSENGINE_H

#include <QObject>
#include <QPoint>

#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

class ControlsEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit ControlsEngine(QOpenGLContext* context, QObject* parent = 0);
  virtual ~ControlsEngine();

  void draw(const QMatrix4x4& mvp_mat);

signals:

public slots:
  inline void setCursorPosition(QPoint pos) { _cursorPos = pos; }

private:
  QPoint _cursorPos;

  void initShaders();

  void initCursorBuffers();
  void initCircleBuffers();
  void initRayBuffers();

  void drawCursor(const QMatrix4x4& mvp_mat);
  void drawCircle(const QMatrix4x4& mvp_mat, float radius);
  void drawRay(const QMatrix4x4& mvp_mat, float angle);

  // -------------------------------------------
  QOpenGLShaderProgram* _prog;
  enum { CTRL_ATTR_ANGLE = 0
       , CTRL_ATTR_RADIUS = 1
       , CTRL_ATTR_COUNT = 2 } ;
  enum { CTRL_UNIF_MVP = 0
       , CTRL_UNIF_COLOR = 1
       , CTRL_UNIF_COUNT = 2 } ;

  GLuint _attr_locs[CTRL_ATTR_COUNT];
  GLuint _unif_locs[CTRL_UNIF_COUNT];

  GLuint _vbo_ids_cursor[CTRL_ATTR_COUNT];
  GLuint _vbo_ids_circle[CTRL_ATTR_COUNT];
  GLuint _vbo_ids_ray[CTRL_ATTR_COUNT];
};

#endif // CONTROLSENGINE_H
