#include "mainwindow.h"

#include <QDesktopWidget>

#include "common/properties.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setGeometry(QGuiApplication::screens()[0]->geometry());

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWindow construction start";

  wgtRLI = new RLIDisplayWidget(this);
  wgtButtonPanel = new RLIControlWidget(wgtRLI, this);

  connect(wgtButtonPanel, SIGNAL(closeApp()), SLOT(close()));

  connect(wgtButtonPanel, SIGNAL(gainChanged(float)), wgtRLI, SLOT(onGainChanged(float)));
  connect(wgtButtonPanel, SIGNAL(waterChanged(float)), wgtRLI, SLOT(onWaterChanged(float)));
  connect(wgtButtonPanel, SIGNAL(rainChanged(float)), wgtRLI, SLOT(onRainChanged(float)));

  connect(wgtRLI, SIGNAL(initialized()), SLOT(onRLIWidgetInitialized()));

  _radar_ds = new RadarDataSource(this);
  //_ship_ds = new ShipDataSource(this);
  //_target_ds = new TargetDataSource(this);

  _radar_ds->start();
  //_ship_ds->start();
  //_target_ds->start();

  wgtRLI->setFocusPolicy(Qt::StrongFocus);
  wgtRLI->setFocus();

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWindow construction finish";
}

MainWindow::~MainWindow() {
  _radar_ds->finish();
  //_ship_ds->finish();
  //_target_ds->finish();

  delete _radar_ds;
  //delete _ship_ds;
  //delete _target_ds;
}

void MainWindow::resizeEvent(QResizeEvent* e) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWindow resizeEvent" << e->size();

  bool showButtonPanel = qApp->property(PROPERTY_SHOW_BUTTON_PANEL).toBool();

  wgtButtonPanel->setVisible(showButtonPanel);
  wgtButtonPanel->move(width() - wgtButtonPanel->width(), 0);

  QSize s = e->size();
  QSize availableSize;  
  int availableWidth;

  if (qApp->property(PROPERTY_RLI_WIDGET_SIZE).isValid()) {
    QStringList l = qApp->property(PROPERTY_RLI_WIDGET_SIZE).toString().split("x");
    availableSize = QSize(l[0].toInt(), l[1].toInt());
  } else {
    availableWidth = showButtonPanel ? s.width() - wgtButtonPanel->width() : s.width();
    availableSize = QSize(availableWidth, s.height() );
  }

  wgtRLI->setGeometry(geometry());

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWindow resizeEvent finish";
}

void MainWindow::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);
  wgtRLI->update();
}

void MainWindow::onRLIWidgetInitialized() {
  wgtRLI->setupRadarDataSource(_radar_ds);
  //wgtRLI->setupTargetDataSource(_target_ds);
  //wgtRLI->setupShipDataSource(_ship_ds);

  int frame = qApp->property(PROPERTY_FRAME_DELAY).toInt();
  startTimer(frame, Qt::CoarseTimer);
}
