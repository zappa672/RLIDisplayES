#ifndef RADARENGINE_H
#define RADARENGINE_H

#include <QTime>
#include <QColor>
#include <QVector2D>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "radarpalette.h"

// Класс для отрисовки радарного круга
class RadarEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  explicit RadarEngine  (uint pel_count, uint pel_len, uint tex_radius, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~RadarEngine  ();

  inline QSize size() const      { return _fbo->size(); }
  inline GLuint textureId() const { return _fbo->texture(); }

  inline GLuint amplitutedesVboId() const { return _vbo_ids[ATTR_AMP]; }
  inline GLuint paletteTexId() const { return _palette->texture(); }

  inline int pelengCount() const { return _peleng_count; }
  inline int pelengLength() const { return _peleng_len; }

public slots:
  void onBrightnessChanged(int br);

  void resizeData     (uint pel_count, uint pel_len);
  void resizeTexture  (uint radius);

  void clearTexture();
  void clearData();

  void updateTexture();
  void updateData(uint offset, uint count, GLfloat* amps);

private:
  void initShader();

  void fillCoordTable();

  void drawPelengs(uint first, uint last);

  bool _has_data;

  // Radar parameters  
  uint    _radius;
  uint    _peleng_count, _peleng_len;

  std::vector<GLuint> _draw_indices;
  std::vector<GLfloat> _positions;

  bool  _draw_circle;
  uint  _last_drawn_peleng, _last_added_peleng;

  // OpenGL vars
  QOpenGLFramebufferObject* _fbo;
  QOpenGLShaderProgram* _program;

  // OpenGL program attributres enum
  enum { ATTR_POS = 0, ATTR_AMP = 1, ATTR_CNT = 2 } ;
  // OpenGL program uniforms enum
  enum { UNIF_MVP = 0, UNIF_TEX = 1, UNIF_THR = 2, UNIF_PLN = 3, UNIF_PCN = 4, UNIF_CNT = 5 } ;

  GLuint _vbo_ids[ATTR_CNT];
  // OpenGL program uniforms locations
  GLuint _unif_locs[UNIF_CNT];
  // OpenGL program attributres locations
  GLuint _attr_locs[ATTR_CNT];
  GLuint _ind_vbo_id;

  // Palette
  RadarPalette* _palette;
};

#endif // RADARENGINE_H
