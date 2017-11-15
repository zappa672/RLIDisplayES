#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "rlicontrolwidget.h"
#include "rlidisplaywidget.h"

#include "datasources/radardatasource.h"

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

  void onRLIWidgetInitialized();

private:
  Ui::MainWindow *ui;

  RadarDataSource* _radar_ds;

  RLIDisplayWidget* wgtRLI;
  RLIControlWidget* wgtButtonPanel;
};

#endif // MAINWINDOW_H
