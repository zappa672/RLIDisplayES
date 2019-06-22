#ifndef CHARTAREAENGINE_H
#define CHARTAREAENGINE_H

#include <QColor>
#include <QVector2D>
#include <QOpenGLVertexArrayObject>

#include "chartshaders.h"

#include "../../s52/s52chart.h"
#include "../../s52/s52assets.h"
#include "../../s52/s52references.h"


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

#endif // CHARTAREAENGINE_H
