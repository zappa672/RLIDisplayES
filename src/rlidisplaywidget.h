#ifndef RLIDISPLAYWIDGET_H
#define RLIDISPLAYWIDGET_H

#include <QQueue>
#include <QWidget>
#include <QMouseEvent>

#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "s52/chartmanager.h"

#include "common/rlilayout.h"
#include "common/rlistate.h"

#include "datasources/radardatasource.h"
#include "datasources/shipdatasource.h"
#include "datasources/targetdatasource.h"

#include "layers/radar/radarengine.h"
#include "layers/chart/chartengine.h"
#include "layers/info/infoengine.h"
#include "layers/info/menuengine.h"
#include "layers/magnifierengine.h"
#include "layers/maskengine.h"
#include "layers/targetengine.h"
#include "layers/controlsengine.h"


class RLIDisplayWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  explicit RLIDisplayWidget(QWidget *parent = 0);
  ~RLIDisplayWidget();

  void toggleRadarTailsShift();
  float frameRate();

  void setupRadarDataSource(RadarDataSource* rds);
  void setupTargetDataSource(TargetDataSource* tds);
  void setupShipDataSource(ShipDataSource* sds);

signals:
  void initialized();

public slots:  
  void keyReleaseEvent(QKeyEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void mousePressEvent(QMouseEvent* event);

  void onGainChanged(float value);
  void onWaterChanged(float value);
  void onRainChanged(float value);
  void onApchChanged(float value);
  void onEmissionChanged(float value);

  void onShipPositionChanged(const std::pair<float,float>& pos);

protected slots:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

private slots:
  void onNewChartAvailable(const QString& name);

private:
  QSet<int> pressedKeys;

  bool _initialized;

  int _debug_radar_tails_shift;

  QMutex frameRateMutex;
  QQueue<QDateTime> frameTimes;

  void debugInfo();
  void initShaders();

  void paintLayers();
  void updateLayers();

  void drawRect(const QRectF& rect, GLuint textureId);

  RLIState _state;
  ChartManager* _chart_mngr;
  RLILayoutManager* _layout_manager;

  InfoFonts*    _infoFonts;

  MaskEngine*       _maskEngine;
  RadarEngine*      _radarEngine;
  RadarEngine*      _tailsEngine;
  ChartEngine*      _chartEngine;
  InfoEngine*       _infoEngine;
  MenuEngine*       _menuEngine;
  TargetEngine*     _trgtEngine;
  ControlsEngine*   _ctrlEngine;
  MagnifierEngine*  _magnEngine;


  QOpenGLShaderProgram* _program;

  enum { ATTR_POSITION = 0, ATTR_TEXCOORD = 1, ATTR_COUNT = 2 } ;
  enum { UNIF_TEXTURE = 0, UNIF_MVPMATRIX = 1, UNIF_COUNT = 2 } ;

  GLuint _vbo_ids[ATTR_COUNT];
  GLuint _unif_locs[UNIF_COUNT];
  GLuint _attr_locs[ATTR_COUNT];  

  QMatrix4x4 _projection;
};

#endif // RLIDISPLAYWIDGET_H
