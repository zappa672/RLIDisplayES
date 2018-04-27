#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>

#include "rlicontrolwidget.h"
#include "rlidisplaywidget.h"

#include "datasources/radardatasource.h"
#include "datasources/shipdatasource.h"
#include "datasources/targetdatasource.h"
#include "datasources/infocontrollers.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

signals:
  void gainChanged(int val);
  void waterChanged(int val);
  void rainChanged(int val);

protected slots:
  void resizeEvent(QResizeEvent* e);
  void timerEvent(QTimerEvent* e);

  void keyReleaseEvent(QKeyEvent *event);
  void keyPressEvent(QKeyEvent *event);

  void onRLIWidgetInitialized();

private:
  void setupInfoBlock(InfoBlockController* ctrl, const RLIPanelInfo& panelInfo);

  Ui::MainWindow *ui;

  QSet<int> pressedKeys;

  RadarDataSource* _radar_ds;
  ShipDataSource* _ship_ds;
  //TargetDataSource* _target_ds;

  RLIDisplayWidget* wgtRLI;
  RLIControlWidget* wgtButtonPanel;

  // Контроллеры инфоблоков
  ValueBarController* _gain_ctrl;
  ValueBarController* _water_ctrl;
  ValueBarController* _rain_ctrl;
  ValueBarController* _apch_ctrl;
  ValueBarController* _rdtn_ctrl;

  CursorController* _curs_ctrl;
  ClockController* _clck_ctrl;
  FpsController* _fps_ctrl;

  ScaleController* _scle_ctrl;
  CourseController* _crse_ctrl;
  DangerController* _dngr_ctrl;

  LabelController* _lbl1_ctrl;
  LabelController* _lbl2_ctrl;
  LabelController* _lbl3_ctrl;
  LabelController* _lbl4_ctrl;
  LabelController* _lbl5_ctrl;
  LabelController* _band_lbl_ctrl;

  PositionController* _pstn_ctrl;
  BlankController* _blnk_ctrl;
  TailsController* _tals_ctrl;
  DangerDetailsController* _dgdt_ctrl;
  VectorController* _vctr_ctrl;
  TargetsController* _trgs_ctrl;

  VnController* _vn_ctrl;
  VdController* _vd_ctrl;
};

#endif // MAINWINDOW_H
