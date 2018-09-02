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

#include "chartsettingsmodel.h"
#include "chartlayers.h"
#include "chartshaders.h"

class ChartEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit ChartEngine(uint tex_radius, S52References* ref, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~ChartEngine();

  void resize(uint radius);
  inline QSize size() { return _fbo->size(); }

  void setChart(S52Chart* chrt, S52References* ref);
  void update(const RLIState& state, const QString& color_scheme);

  inline GLuint textureId() { return _fbo->texture(); }
  //inline void forceUpdate() { _force_update = true; }

private:
  void clearChartData();

  bool _ready;
  bool _force_update;

  QOpenGLContext* _context;

  ChartShaders* shaders;
  ChartSettingsModel* settings;

  uint    _radius;

  QPoint _center_shift { 0, 0 };
  QVector2D _center { 0, 0 };
  float _scale { 10 };
  float _angle { 0 };

  S52Assets* assets;  
  QOpenGLFramebufferObject* _fbo = nullptr;

  void draw(const QString& color_scheme);

  void drawAreaLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme);
  void drawLineLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme);
  //void drawTextLayers(const QMatrix4x4& mvp_matrix);
  void drawMarkLayers(const QMatrix4x4& mvp_matrix, const QString& color_scheme);

  void setAreaLayers(S52Chart* chrt, S52References* ref);
  void setLineLayers(S52Chart* chrt, S52References* ref);
  //void setTextLayers(S52Chart* chrt, S52References* ref);
  void setMarkLayers(S52Chart* chrt, S52References* ref);
  void setSndgLayer(S52Chart* chrt, S52References* ref);

  QMap<QString, ChartAreaEngine*>  area_engines;
  QMap<QString, ChartLineEngine*>  line_engines;
  //QMap<QString, ChartTextEngine*>  text_engines;
  QMap<QString, ChartMarkEngine*>  mark_engines;
};

#endif // CHARTENGINE_H
