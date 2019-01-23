#ifndef RADARENGINE_H
#define RADARENGINE_H

#include <QTime>
#include <QColor>
#include <QVector2D>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../../common/rlistate.h"
#include "radarpalette.h"

// Класс для отрисовки радарного круга
class RadarEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  RadarEngine  (int pel_count, int pel_len, int tex_radius, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~RadarEngine  ();

  inline QSize size()           const { return _fbo->size(); }
  inline GLuint textureId()     const { return _fbo->texture(); }

  inline GLuint ampsVboId()     const { return _vbo_ids[ATTR_AMPLITUDE]; }
  inline GLuint paletteTexId()  const { return _palette->texture(); }

  inline int pelengCount()      const { return _peleng_count; }
  inline int pelengLength()     const { return _peleng_len; }

public slots:
  void onBrightnessChanged(int br);

  void resizeData     (int pel_count, int pel_len);
  void resizeTexture  (int radius);

  void clearTexture();
  void clearData();

  void updateTexture(const RLIState& _rli_state);
  void updateData(int offset, int count, GLfloat* amps);

private:
  void initShader();

  void fillCoordTable();

  void drawPelengs(int first, int last);

  bool _has_data = false;

  // Radar parameters
  int _peleng_count = 0;
  int _peleng_len   = 0;

  std::vector<GLuint> _draw_indices;
  std::vector<GLfloat> _positions;

  bool  _draw_circle;
  int  _last_drawn_peleng, _last_added_peleng;
  QPointF _center_shift { 0.0, 0.0 };

  // OpenGL vars
  QOpenGLFramebufferObject* _fbo = nullptr;
  QOpenGLShaderProgram* _program = new QOpenGLShaderProgram(this);

  // OpenGL program attributres enum
  enum { ATTR_POSITION  = 0
       , ATTR_AMPLITUDE = 1
       , ATTR_COUNT     = 2 } ;

  // OpenGL program uniforms enum
  enum { UNIF_MVP_MATRIX    = 0
       , UNIF_TEXTURE       = 1
       , UNIF_THREASHOLD    = 2
       , UNIF_PELENG_LENGTH = 3
       , UNIF_PELENG_COUNT  = 4
       , UNIF_FBO_RADIUS    = 5
       , UNIF_NORTH_SHIFT   = 6
       , UNIF_COUNT         = 7 } ;

  GLuint _vbo_ids[ATTR_COUNT];
  // OpenGL program uniforms locations
  int _unif_locs[UNIF_COUNT];
  // OpenGL program attributres locations
  int _attr_locs[ATTR_COUNT];
  GLuint _ind_vbo_id;

  // Palette
  RadarPalette* _palette;
};

#endif // RADARENGINE_H
