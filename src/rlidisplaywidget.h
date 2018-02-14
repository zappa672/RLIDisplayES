#ifndef RLIDISPLAYWIDGET_H
#define RLIDISPLAYWIDGET_H

#include <QWidget>
#include <QMouseEvent>

#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "s52/chartmanager.h"

#include "datasources/radarscale.h"

#include "layers/radarengine.h"
#include "layers/maskengine.h"
#include "layers/chartengine.h"
#include "layers/infoengine.h"


class RLIDisplayWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  explicit RLIDisplayWidget(QWidget *parent = 0);
  ~RLIDisplayWidget();

  inline RadarEngine* radarEngine() { return _radarEngine; }
  inline RadarEngine* tailsEngine() { return _tailsEngine; }
  inline InfoEngine* infoEngine() { return _infoEngine; }

  void toggleRadarTailsShift();

signals:
  void initialized();

protected slots:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

private slots:
  void onNewChartAvailable(const QString& name);

private:
  bool _initialized;

  int _debug_radar_tails_shift;

  void debugInfo();
  void initShaders();

  void paintLayers();
  void updateLayers();

  void drawRect(const QRectF& rect, GLuint textureId);

  ChartManager* _chart_mngr;

  InfoFonts* _infoFonts;

  MaskEngine* _maskEngine;
  RadarEngine* _radarEngine;
  RadarEngine* _tailsEngine;
  ChartEngine* _chartEngine;
  InfoEngine*  _infoEngine;

  QOpenGLShaderProgram* _program;

  enum { ATTR_POSITION = 0, ATTR_TEXCOORD = 1, ATTR_COUNT = 2 } ;
  enum { UNIF_TEXTURE = 0, UNIF_MVPMATRIX = 1, UNIF_COUNT = 2 } ;

  GLuint _vbo_ids[ATTR_COUNT];
  GLuint _unif_locs[UNIF_COUNT];
  GLuint _attr_locs[ATTR_COUNT];

  QMatrix4x4 _projection;
};

#endif // RLIDISPLAYWIDGET_H
