#ifndef CONTROLSENGINE_H
#define CONTROLSENGINE_H

#include <QObject>
#include <QPoint>
#include <QColor>

#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "../common/rlistate.h"


class ControlsEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit ControlsEngine(QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~ControlsEngine();

  void draw(const QMatrix4x4& mvp_mat, const RLIState& state);

private:
  const QColor RLI_CNTR_COLOR_PAR_LINES  { 255, 255, 255, 255 };
  const QColor RLI_CNTR_COLOR_CAPT_AREA  { 255, 255,   0, 255 };
  const QColor RLI_CNTR_COLOR_MAGNIFIER  {   0,   0, 255, 255 };
  const QColor RLI_CNTR_COLOR_CIRCLES    { 203,  67,  69, 255 };
  const QColor RLI_CNTR_COLOR_CURSOR     { 255,   0, 255, 255 };
  const QColor RLI_CNTR_COLOR_VN_CU      { 255, 192,  26, 255 };
  const QColor RLI_CNTR_COLOR_CU         {   0, 255,   0, 255 };
  const QColor RLI_CNTR_COLOR_VN_P       { 255, 254, 255, 255 };
  const QColor RLI_CNTR_COLOR_VD         { 255, 255, 255, 255 };

  const size_t RLI_CNTR_CIRCLE_RESOLUTION = 4096;


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

  int _attr_locs[CTRL_ATTR_COUNT];
  int _unif_locs[CTRL_UNIF_COUNT];

  GLuint _vbo_ids_cursor[CTRL_ATTR_COUNT];
  GLuint _vbo_ids_circle[CTRL_ATTR_COUNT];
  GLuint _vbo_ids_ray[CTRL_ATTR_COUNT];
};

#endif // CONTROLSENGINE_H
