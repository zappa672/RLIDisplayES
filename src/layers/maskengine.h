#ifndef MASKENGINE_H
#define MASKENGINE_H

#define CIRCLE_RAY_COUNT 720
#define MARK_RAY_COUNT 36

#include <QVector>
#include <QMap>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLShaderProgram>

#include "../common/rlilayout.h"
#include "info/infofonts.h"



class MaskEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT
public:
  explicit MaskEngine (const QSize& sz, const RLICircleLayout& layout, InfoFonts* fonts, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~MaskEngine ();

  void resize(const QSize& sz, const RLICircleLayout& layout);

  inline GLuint textureId()   { return _fbo->texture(); }

  inline void setCursorPos  (const QPoint& p)   { _cursor_pos = p; }

  inline GLuint  getTextureId   ()   { return _fbo->texture(); }

public slots:
  void update();

private:
  void initBuffers();
  void initShader();

  void initRectBuffers();
  void initLineBuffers();
  void initTextBuffers();
  void initHoleBuffers();

  void bindBuffers(GLuint* vbo_ids);
  void setBuffers(GLuint* vbo_ids, int count, GLfloat* angles, GLfloat* chars, GLfloat* orders, GLfloat* shifts);

  InfoFonts* _fonts;

  float     _hole_radius;

  QPointF   _hole_center;
  QPointF   _cursor_pos;

  QString   _font;

  QOpenGLFramebufferObject* _fbo;
  QOpenGLShaderProgram* _program;

  enum { MASK_ATTR_ANGLE = 0
       , MASK_ATTR_CHAR_VAL = 1
       , MASK_ATTR_ORDER = 2
       , MASK_ATTR_SHIFT = 3
       , MASK_ATTR_COUNT = 4 } ;
  enum { MASK_UNIF_MVP = 0
       , MASK_UNIF_ANGLE_SHIFT = 1
       , MASK_UNIF_CIRCLE_RADIUS = 2
       , MASK_UNIF_CIRCLE_POS = 3
       , MASK_UNIF_CURSOR_POS = 4
       , MASK_UNIF_COLOR = 5
       , MASK_UNIF_FONT_SIZE = 6
       , MASK_UNIF_GLYPH_TEX = 7
       , MASK_UNIF_COUNT = 8 } ;

  GLuint vbo_ids_mark  [MASK_ATTR_COUNT];

  GLuint _ind_vbo_id_text;
  GLuint vbo_ids_text  [MASK_ATTR_COUNT];
  int _text_point_count;

  GLuint vbo_ids_hole  [MASK_ATTR_COUNT];
  int _hole_point_count;

  GLuint _unif_locs[MASK_UNIF_COUNT];
  GLuint _attr_locs[MASK_ATTR_COUNT];
};
#endif // MASKENGINE_H
