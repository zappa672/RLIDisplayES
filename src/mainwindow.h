#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>

#include "rlicontrolwidget.h"
#include "rlidisplaywidget.h"

#include "datasources/radardatasource.h"
#include "datasources/shipdatasource.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected slots:
  void resizeEvent(QResizeEvent* e);
  void timerEvent(QTimerEvent* e);

  void keyReleaseEvent(QKeyEvent *event);
  void keyPressEvent(QKeyEvent *event);

  void onRLIWidgetInitialized();

private:
  Ui::MainWindow *ui;

  QSet<int> pressedKeys;

  RadarDataSource* _radar_ds;
  ShipDataSource* _ship_ds;

  RLIDisplayWidget* wgtRLI;
  RLIControlWidget* wgtButtonPanel;
};

#endif // MAINWINDOW_H
