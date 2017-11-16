#ifndef RADARENGINE_H
#define RADARENGINE_H

#include <QTime>
#include <QColor>
#include <QVector2D>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "radarpalette.h"

// Класс для отрисовки радарного круга
class RadarEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  explicit RadarEngine  (uint pel_count, uint pel_len, uint circle_radius, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~RadarEngine  ();

  void resizeData     (uint pel_count, uint pel_len);
  void resizeTexture  (uint radius);

  inline QSize size() const          { return _fbo->size(); }
  inline uint  textureId() const     { return _fbo->texture(); }

public slots:
  void clearTexture();
  void clearData();

  void onBrightnessChanged(int br);

  void updateTexture();
  void updateData(uint offset, uint count, GLfloat* amps);

private:
  void initShader();
  void fillCoordTable();

  void drawPelengsShifted(uint first_pos, uint first_amp, uint count);
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

  enum { ATTR_POS = 0, ATTR_AMP = 1, ATTR_CNT = 2 } ;
  enum { UNIF_MVP = 0, UNIF_TEX = 1, UNIF_THR = 2, UNIF_PLN = 3, UNIF_PCN = 4, UNIF_CNT = 5 } ;

  GLuint _vbo_ids[ATTR_CNT];
  GLuint _unif_locs[UNIF_CNT];
  GLuint _attr_locs[ATTR_CNT];

  GLuint _ind_vbo_id;

  // Palette
  RadarPalette* _palette;
};

#endif // RADARENGINE_H
