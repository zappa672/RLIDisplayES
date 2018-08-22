#ifndef CONTROLSENGINE_H
#define CONTROLSENGINE_H

#include <QObject>
#include <QPoint>
#include <QColor>

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

  inline float getVnP() { return _vn_p; }
  inline float getVnCu() { return _vn_cu; }
  inline float getVd() { return _vd; }

  inline void shiftVnP(float d) { _vn_p += d; }
  inline void shiftVnCu(float d) {_vn_cu += d; if(_vn_cu < 0) _vn_cu = 360 + _vn_cu; if(_vn_cu >= 360) _vn_cu = _vn_cu - 360;}
  inline void shiftVd(float d) { _vd += d; if (_vd < 0) _vd = 0; }

  bool isCirclesVisible() { return _showCircles; }
  bool isParallelLinesVisible() { return _showParallelLines; }

signals:

public slots:
  inline void setCursorPosition(QPoint pos) { _cursorPos = pos; }
  inline void setCirclesVisible(bool val) { _showCircles = val; }
  inline void setParallelLinesVisible(bool val) { _showParallelLines = val; }

private:
  float _vn_p;
  float _vn_cu;
  float _vd;

  QPoint _cursorPos;

  bool _showCircles;
  bool _showParallelLines;

  void initShaders();

  void initCursorBuffers();
  void initCircleBuffers();

  void drawCursor(const QColor& col);
  void drawCircleSegment(const QColor& col, GLfloat radius, GLfloat min_angle = 0.f, GLfloat max_angle = 360.f);
  void drawRaySegment(const QColor& col, GLfloat angle, GLfloat min_radius = 0.f, GLfloat max_radius = 2048.f, GLfloat shift = 0.f);

  // -------------------------------------------
  QOpenGLShaderProgram* _prog;
  enum { CTRL_ATTR_ANGLE = 0
       , CTRL_ATTR_RADIUS = 1
       , CTRL_ATTR_COUNT = 2 } ;
  enum { CTRL_UNIF_MVP = 0
       , CTRL_UNIF_SHIFT = 1
       , CTRL_UNIF_COLOR = 2
       , CTRL_UNIF_COUNT = 3 } ;

  GLuint _attr_locs[CTRL_ATTR_COUNT];
  GLuint _unif_locs[CTRL_UNIF_COUNT];

  GLuint _vbo_ids_cursor[CTRL_ATTR_COUNT];
  GLuint _vbo_ids_circle[CTRL_ATTR_COUNT];
  GLuint _vbo_ids_ray[CTRL_ATTR_COUNT];
};

#endif // CONTROLSENGINE_H
