#ifndef CHARTLAYERENGINES_H
#define CHARTLAYERENGINES_H

#include <QColor>
#include <QVector2D>
#include <QOpenGLVertexArrayObject>

#include "chartshaders.h"

#include "../s52/s52chart.h"
#include "../s52/s52assets.h"
#include "../s52/s52references.h"

class ChartAreaEngine : protected QOpenGLFunctions {
public:
  explicit ChartAreaEngine(QOpenGLContext* context);
  virtual ~ChartAreaEngine();

  void clearData();
  void setData(S52AreaLayer* layer, S52Assets* assets, S52References* ref);

  void draw(ChartShaders* shaders);

private:
  GLuint vbo_ids[AREA_ATTRIBUTES_COUNT];

  int point_count;

  bool is_pattern_uniform;
  QPointF patternLocation;
  QSizeF patternSize;

  bool is_color_uniform;
  GLfloat color_ind;
};


class ChartLineEngine : protected QOpenGLFunctions {
public:
  explicit ChartLineEngine(QOpenGLContext* context);
  virtual ~ChartLineEngine();

  void clearData();
  void setData(S52LineLayer* layer, S52Assets* assets, S52References* ref);

  void draw(ChartShaders* shaders);

private:
  GLuint point_count;

  bool is_pattern_uniform;
  QPoint patternIdx;
  QSize  patternDim;

  bool is_color_uniform;
  GLfloat color_ind;

  GLuint  vbo_ids[LINE_ATTRIBUTES_COUNT];
  GLuint _ind_vbo_id;
};


class ChartMarkEngine : protected QOpenGLFunctions {
public:
  explicit ChartMarkEngine(QOpenGLContext* context);
  virtual ~ChartMarkEngine();

  void clearData();
  void setData(S52MarkLayer* layer, S52References* ref);

  void draw(ChartShaders* shaders);

private:
  GLuint vbo_ids[MARK_ATTRIBUTES_COUNT];
  GLuint _ind_vbo_id;

  GLuint point_count;

  bool is_pattern_uniform;
  QPointF patternOrigin;
  QSizeF patternSize;
  QPointF patternPivot;
};






/*
class ChartSndgEngine : protected QOpenGLFunctions {
public:
  explicit ChartSndgEngine(QOpenGLContext* context);
  virtual ~ChartSndgEngine();

  void clearData();

  void setPatternTexture(GLuint tex_id, QVector2D size);
  void setData(S52SndgLayer* layer, S52Assets* assets, S52References* ref);

  void draw(ChartShaders* shaders, std::pair<float, float> cur_coords, float scale, float angle, const QMatrix4x4& mvp);

private:
  GLuint point_count;

  GLuint* vbo_ids;
  GLuint _ind_vbo_id;

  GLint     pattern_tex_id;
  QVector2D pattern_tex_size;
};


class ChartTextEngine : protected QOpenGLFunctions {
public:
  explicit ChartTextEngine(QOpenGLContext* context);
  virtual ~ChartTextEngine();

  void clearData();

  void setGlyphTexture(GLuint tex_id);
  void setData(S52TextLayer* layer);

  void draw(ChartShaders* shaders, std::pair<float, float> cur_coords, float scale, float angle, const QMatrix4x4& mvp);

private:
  int point_count;

  GLuint* vbo_ids;
  GLint   glyph_tex_id;
};
*/

#endif // CHARTLAYERENGINES_H
