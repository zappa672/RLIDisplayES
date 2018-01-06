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

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  pressedKeys.remove(event->key());
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch(event->key()) {
  //Под. имп. Помех
  case Qt::Key_S:
    break;
  //Меню
  case Qt::Key_W:
    break;
  //Шкала +
  case Qt::Key_Plus:
    break;
  //Шкала -
  case Qt::Key_Minus:
    break;
  //Вынос центра
  case Qt::Key_C:
    break;
  //Скрытое меню
  case Qt::Key_U:
    break;
  //Следы точки
  case Qt::Key_T:
    break;
  //Выбор цели
  case Qt::Key_Up:
    break;
  //ЛИД / ЛОД
  case Qt::Key_Down:
    break;
  //Захват
  case Qt::Key_Enter:
    break;
  //Захват
  case Qt::Key_Return:
    break;
  //Сброс
  case Qt::Key_Escape:
    break;
  //Парал. Линии
  case Qt::Key_Backslash:
    break;
  //Электронная лупа
  case Qt::Key_L:
    break;
  //Обзор
  case Qt::Key_X:
    break;
  //Узкий / Шир.
  case Qt::Key_Greater:
    break;
  //Накоп. Видео
  case Qt::Key_V:
    break;
  //Сброс АС
  case Qt::Key_Q:
    break;
  //Манёвр
  case Qt::Key_M:
    break;
  //Курс / Север / Курс стаб
  case Qt::Key_H:
    break;
  //ИД / ОД
  case Qt::Key_R:
    break;
  //НКД
  case Qt::Key_D:
    break;
  //Карта (Маршрут)
  case Qt::Key_A:
    break;
  //Выбор
  case Qt::Key_G:
    break;
  //Стоп-кадр
  case Qt::Key_F:
    break;
  //Откл. Звука
  case Qt::Key_B:
    break;
  //Откл. ОК
  case Qt::Key_K:
    break;
  //Вынос ВН/ВД
  case Qt::Key_Slash:
    break;
  }

  QWidget::keyPressEvent(event);
  pressedKeys.insert(event->key());
}
