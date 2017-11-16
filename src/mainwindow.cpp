#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/rliconfig.h"
#include "common/properties.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  wgtRLI = new RLIDisplayWidget(this);
  wgtButtonPanel = new RLIControlWidget(this);

  connect(wgtButtonPanel, SIGNAL(closeApp()), SLOT(close()));
  connect(wgtRLI, SIGNAL(initialized()), SLOT(onRLIWidgetInitialized()));

  _radar_ds = new RadarDataSource();
}

MainWindow::~MainWindow() {
  _radar_ds->finish();

  delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* e) {
  bool showButtonPanel = RLIConfig::instance().showButtonPanel();
  wgtButtonPanel->setVisible(showButtonPanel);
  wgtButtonPanel->move( width() - wgtButtonPanel->width(), 0 );

  QSize s = e->size();
  QSize availableSize;

  if (qApp->property(PROPERTY_RLI_WIDGET_SIZE).isValid()) {
    QStringList l = qApp->property(PROPERTY_RLI_WIDGET_SIZE).toString().split("x");
    availableSize = QSize(l[0].toInt(), l[1].toInt());
  } else {
    availableSize = QSize(showButtonPanel ? s.width() - wgtButtonPanel->width() : s.width(), s.height());
  }

  RLIConfig::instance().setCurrentSize(availableSize);
  QSize curSize = RLIConfig::instance().currentSize();

  wgtRLI->setGeometry(QRect(QPoint(0, 0), curSize));
}

void MainWindow::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  wgtRLI->update();
}

void MainWindow::onRLIWidgetInitialized() {
  connect( _radar_ds, SIGNAL(updateData(uint,uint,GLfloat*))
         , wgtRLI->radarEngine(), SLOT(updateData(uint,uint,GLfloat*)));
  connect( _radar_ds, SIGNAL(updateData2(uint,uint,GLfloat*))
         , wgtRLI->radarEngine2(), SLOT(updateData(uint,uint,GLfloat*)));

  _radar_ds->start();

  int frame = qApp->property(PROPERTY_FRAME_DELAY).toInt();
  startTimer(frame, Qt::CoarseTimer);
}
