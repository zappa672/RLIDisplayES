#ifndef CHARTTEXTENGINE_H
#define CHARTTEXTENGINE_H

#include <QColor>
#include <QVector2D>
#include <QOpenGLVertexArrayObject>

#include "chartshaders.h"

#include "../../s52/s52chart.h"
#include "../../s52/s52assets.h"
#include "../../s52/s52references.h"


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


#endif // CHARTTEXTENGINE_H
