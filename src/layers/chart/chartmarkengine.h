#ifndef CHARTMARKENGINE_H
#define CHARTMARKENGINE_H

#include <QColor>
#include <QVector2D>
#include <QOpenGLVertexArrayObject>

#include "chartshaders.h"

#include "../../s52/s52chart.h"
#include "../../s52/s52assets.h"
#include "../../s52/s52references.h"


class ChartMarkEngine : protected QOpenGLFunctions {
public:
  explicit ChartMarkEngine(QOpenGLContext* context);
  virtual ~ChartMarkEngine();

  void clearData();
  void setData(S52::MarkLayer* layer, S52References* ref, int display_order);
  void setData(S52::SndgLayer* layer, S52Assets* assets, S52References* ref, int display_order);

  void draw(ChartShaders* shaders);
  inline int displayOrder() { return _display_order; }

private:
  void setupBuffers( const std::vector<GLfloat>& world_coords
                   , const std::vector<GLfloat>& vertex_offsets
                   , const std::vector<GLfloat>& tex_coords );

  GLuint vbo_ids[MARK_ATTR_COUNT];
  GLuint _ind_vbo_id;

  GLuint point_count;
  int _display_order;
};


#endif // CHARTMARKENGINE_H
