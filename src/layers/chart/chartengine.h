#ifndef CHARTENGINE_H
#define CHARTENGINE_H

#include <QObject>
#include <QVector2D>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../../common/rlistate.h"

#include "../../s52/s52assets.h"
#include "../../s52/s52chart.h"
#include "../../s52/s52references.h"

#include "chartareaengine.h"
#include "chartlineengine.h"
#include "charttextengine.h"
#include "chartmarkengine.h"
#include "chartshaders.h"

class ChartEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  ChartEngine(int tex_radius, S52References* ref, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~ChartEngine();

  void resize(int radius);
  inline QSize size() { return _fbo->size(); }

  void setChart(S52::Chart* chrt, S52References* ref);
  void update(const RLIState& state, const QString& color_scheme);

  inline GLuint textureId() { return _fbo->texture(); }

private:
  void clearChartData();

  bool _ready;
  bool _force_update;

  QOpenGLContext* _context;

  ChartShaders* shaders;

  int    _radius;

  QPoint _center_shift  { 0, 0 };
  GeoPos _center        { 0, 0 };
  double _scale         { 10 };
  double _angle         { 0 };

  S52Assets* assets;  
  QOpenGLFramebufferObject* _fbo = nullptr;

  void draw(const QString& color_scheme);

  void drawAreaLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme);
  void drawLineLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme);
  void drawTextLayers(const QMatrix4x4& mvp_matrix);
  void drawMarkLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme);

  void setAreaLayers(S52::Chart* chrt, S52References* ref);
  void setLineLayers(S52::Chart* chrt, S52References* ref);
  void setTextLayers(S52::Chart* chrt, S52References* ref);
  void setMarkLayers(S52::Chart* chrt, S52References* ref);
  void setSndgLayer(S52::Chart* chrt, S52References* ref);

  QVector<ChartAreaEngine*>  area_engines;
  QVector<ChartLineEngine*>  line_engines;
  QVector<ChartTextEngine*>  text_engines;
  QVector<ChartMarkEngine*>  mark_engines;
};

#endif // CHARTENGINE_H
