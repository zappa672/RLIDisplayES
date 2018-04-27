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
  void setData(S52AreaLayer* layer, S52Assets* assets, S52References* ref, int display_order);

  void draw(ChartShaders* shaders);
  inline int displayOrder() { return _display_order; }

private:
  GLuint _vbo_ids[AREA_ATTRIBUTES_COUNT];

  int _point_count;
  int _display_order;

  bool _is_pattern_uniform;
  QPointF _patternLocation;
  QSizeF _patternSize;

  bool _is_color_uniform;
  GLfloat _color_ind;
};


class ChartLineEngine : protected QOpenGLFunctions {
public:
  explicit ChartLineEngine(QOpenGLContext* context);
  virtual ~ChartLineEngine();

  void clearData();
  void setData(S52LineLayer* layer, S52Assets* assets, S52References* ref, int display_order);

  void draw(ChartShaders* shaders);
  inline int displayOrder() { return _display_order; }

private:
  GLuint  vbo_ids[LINE_ATTRIBUTES_COUNT];
  GLuint _ind_vbo_id;

  GLuint point_count;
  int _display_order;

  bool is_pattern_uniform;
  QPoint patternIdx;
  QSize  patternDim;

  bool is_color_uniform;
  GLfloat color_ind;
};


class ChartTextEngine : protected QOpenGLFunctions {
public:
  explicit ChartTextEngine(QOpenGLContext* context);
  virtual ~ChartTextEngine();

  void clearData();
  void setData(S52TextLayer* layer, int display_order);

  void draw(ChartShaders* shaders);
  inline int displayOrder() { return _display_order; }

private:
  GLuint point_count;
  int _display_order;

  GLuint _ind_vbo_id;
  GLuint vbo_ids[TEXT_ATTRIBUTES_COUNT];
};


class ChartMarkEngine : protected QOpenGLFunctions {
public:
  explicit ChartMarkEngine(QOpenGLContext* context);
  virtual ~ChartMarkEngine();

  void clearData();
  void setData(S52MarkLayer* layer, S52References* ref, int display_order);
  void setData(S52SndgLayer* layer, S52Assets* assets, S52References* ref, int display_order);

  void draw(ChartShaders* shaders);
  inline int displayOrder() { return _display_order; }

private:
  void setupBuffers( const std::vector<GLfloat>& world_coords
                   , const std::vector<GLfloat>& vertex_offsets
                   , const std::vector<GLfloat>& tex_coords );

  GLuint vbo_ids[MARK_ATTRIBUTES_COUNT];
  GLuint _ind_vbo_id;

  GLuint point_count;
  int _display_order;
};

#endif // CHARTLAYERENGINES_H
