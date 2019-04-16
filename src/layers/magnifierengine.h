#ifndef MAGNIFIERENGINE_H
#define MAGNIFIERENGINE_H

#include <QDebug>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QMap>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "../common/rlilayout.h"


class MagnifierEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  MagnifierEngine (const RLIMagnifierLayout& layout, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~MagnifierEngine ();

  inline QRect geometry()   { return _geometry; }
  inline GLuint texture()   { return _fbo->texture(); }

  inline void setAmplitudesVBOId(GLuint amp_vbo_id) { _amp_vbo_id = amp_vbo_id; }
  inline void setPalletteTextureId(GLuint pal_tex_id) { _pal_tex_id = pal_tex_id; }

  void resize(const RLIMagnifierLayout& params);

private slots:

public slots:
  void update(int pel_len, int pel_cnt, int min_pel, int min_rad);

private:
  void initShaders();
  void initBorderBuffers();
  void initRadarBuffers();

  void drawBorder();
  void drawPelengs(int pel_len, int pel_cnt, int min_pel, int min_rad);

  GLuint _amp_vbo_id;
  GLuint _pal_tex_id;

  QRect _geometry;
  QOpenGLFramebufferObject* _fbo;
  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------

  enum { MAGN_ATTR_POSITION = 0
       , MAGN_ATTR_AMPLITUDE = 1
       , MAGN_ATTR_COUNT = 2 } ;
  enum { MAGN_UNIF_MVP = 0
       , MAGN_UNIF_COLOR = 1
       , MAGN_UNIF_TEXTURE = 2
       , MAGN_UNIF_THREASHOLD = 3
       , MAGN_UNIF_COUNT = 4 } ;

  GLuint _vbo_ids_border[MAGN_ATTR_COUNT];
  GLuint _vbo_ids_radar[MAGN_ATTR_COUNT];
  GLuint _attr_locs[MAGN_ATTR_COUNT];
  int _unif_locs[MAGN_UNIF_COUNT];
};


#endif // MAGNIFIERENGINE_H
