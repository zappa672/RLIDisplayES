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
  explicit RLIDisplayWidget(QWidget *parent = nullptr);
  ~RLIDisplayWidget() override;

public slots:  
  void keyReleaseEvent(QKeyEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

  void onGainChanged(float value);
  void onWaterChanged(float value);
  void onRainChanged(float value);
  void onApchChanged(float value);
  void onEmissionChanged(float value);

protected slots:  
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void timerEvent(QTimerEvent* e) override;

private slots:
  void onNewChartAvailable(const QString& name);

  void onShipStateChanged(const RLIShipState& sst);

  void onRouteEditionStarted();
  void onRouteEditionFinished();

private:
  float frameRate();

  QSet<int> pressedKeys;

  bool _initialized = false;

  QMutex frameRateMutex;
  QQueue<QDateTime> frameTimes;

  void debugInfo();
  void initShaders();
  void initModeTextures(const QString& path);

  void paintLayers();
  void updateLayers();

  void drawRect(const QRectF& rect, GLuint textureId);

  RLIState _state;

  //ChartManager     _chart_mngr      { this };
  RLILayoutManager _layout_manager  { "layouts.xml" };

  /*
  InfoFonts*        _infoFonts;

  MaskEngine*       _maskEngine;
  */
  RadarEngine*      _radarEngine;
  RadarEngine*      _tailsEngine;
  /*
  ChartEngine*      _chartEngine;
  InfoEngine*       _infoEngine;
  MenuEngine*       _menuEngine;
  TargetEngine*     _trgtEngine;
  RouteEngine*      _routeEngine;
  ControlsEngine*   _ctrlEngine;
  */
  MagnifierEngine*  _magnEngine;

  QMap<char, QOpenGLTexture*> _mode_textures;

  QOpenGLShaderProgram* _program;

  enum { ATTR_POSITION = 0, ATTR_TEXCOORD = 1, ATTR_COUNT = 2 } ;
  enum { UNIF_TEXTURE = 0, UNIF_MVPMATRIX = 1, UNIF_COUNT = 2 } ;

  GLuint _vbo_ids[ATTR_COUNT];
  int _unif_locs[UNIF_COUNT];
  int _attr_locs[ATTR_COUNT];

  QMatrix4x4 _projection;

  RadarDataSource*    _radar_ds;
  /*
  ShipDataSource*     _ship_ds;
  TargetDataSource*   _target_ds;
  */
};

#endif // RLIDISPLAYWIDGET_H
