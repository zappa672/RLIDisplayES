#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>

#include "rlicontrolwidget.h"
#include "rlidisplaywidget.h"

#include "datasources/radardatasource.h"
#include "datasources/shipdatasource.h"
#include "datasources/targetdatasource.h"

Q_DECLARE_METATYPE(RLITarget)
Q_DECLARE_METATYPE(RLIShipState)

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected slots:
  void resizeEvent(QResizeEvent* e);
  void timerEvent(QTimerEvent* e);

  void onRLIWidgetInitialized();

private:
  RadarDataSource*    _radar_ds;
  /*
  ShipDataSource*     _ship_ds;
  TargetDataSource*   _target_ds;
  */

  RLIDisplayWidget*   wgtRLI;
  //RLIControlWidget*   wgtButtonPanel;
};

#endif // MAINWINDOW_H
