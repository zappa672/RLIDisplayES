#ifndef CHARTLINEENGINE_H
#define CHARTLINEENGINE_H

#include <QColor>
#include <QVector2D>
#include <QOpenGLVertexArrayObject>

#include "chartshaders.h"

#include "../../s52/s52chart.h"
#include "../../s52/s52assets.h"
#include "../../s52/s52references.h"


class ChartLineEngine : protected QOpenGLFunctions {
public:
  explicit ChartLineEngine(QOpenGLContext* context);
  virtual ~ChartLineEngine();

  void clearData();
  void setData(S52::LineLayer* layer, S52Assets* assets, S52References* ref, int display_order);

  void draw(ChartShaders* shaders);
  inline int displayOrder() { return _display_order; }

private:
  GLuint  vbo_ids[LINE_ATTR_COUNT];
  GLuint _ind_vbo_id;

  GLuint point_count;
  int _display_order;
};


#endif // CHARTLINEENGINE_H
