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


class MagnifierEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit MagnifierEngine (const QMap<QString, QString>& params, QOpenGLContext* context, QObject* parent = 0);
  virtual ~MagnifierEngine ();

  inline bool visible()     { return _visible; }
  inline void setVisible(bool val) { _visible = val; }

  inline QPoint position()  { return _position; }
  inline QSize  size()      { return _size; }
  inline GLuint texture()   { return _fbo->texture(); }

  void resize (const QMap<QString, QString>& params);

private slots:

public slots:
  void update(GLuint amp_vbo_id, GLuint pal_tex_id, int pel_len, int pel_cnt, int min_pel, int min_rad);

private:
  void initShaders();
  void initBorderBuffers();
  void initRadarBuffers();

  void drawBorder();
  void drawPelengs(GLuint amp_vbo_id, GLuint pal_tex_id, int pel_len, int pel_cnt, int min_pel, int min_rad);

  bool _visible;
  QPoint _position;
  QSize _size;

  QOpenGLFramebufferObject* _fbo;
  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------

  enum { MAGN_ATTR_POSITION = 0
       , MAGN_ATTR_AMPLITUDE = 1
       , MAGN_ATTR_COUNT = 2 } ;
  enum { MAGN_UNIF_MVP = 0
       , MAGN_UNIF_COLOR = 1
       , MAGN_UNIF_TEXTURE = 2
       , MAGN_UNIF_COUNT = 3 } ;

  GLuint _vbo_ids_border[MAGN_ATTR_COUNT];
  GLuint _vbo_ids_radar[MAGN_ATTR_COUNT];
  GLuint _attr_locs[MAGN_ATTR_COUNT];
  GLuint _unif_locs[MAGN_UNIF_COUNT];
};


#endif // MAGNIFIERENGINE_H