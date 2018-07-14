#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/properties.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  wgtRLI = new RLIDisplayWidget(this);
  wgtButtonPanel = new RLIControlWidget(this);

  connect(wgtButtonPanel, SIGNAL(closeApp()), SLOT(close()));
  connect(wgtRLI, SIGNAL(initialized()), SLOT(onRLIWidgetInitialized()));

  _radar_ds = new RadarDataSource(this);
  _ship_ds = new ShipDataSource(this);
  _target_ds = new TargetDataSource(this);

  _radar_ds->start();
  _ship_ds->start();
  _target_ds->start();

  RLIState::instance().onShipPositionChanged(_ship_ds->getPosition());

  connect(_ship_ds, SIGNAL(positionChanged(std::pair<float,float>))
         ,&RLIState::instance(), SLOT(onShipPositionChanged(std::pair<float,float>)));
}

MainWindow::~MainWindow() {
  _radar_ds->finish();
  _ship_ds->finish();
  _target_ds->finish();

  delete _radar_ds;
  delete _ship_ds;
  delete _target_ds;

  delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* e) {
  bool showButtonPanel = qApp->property(PROPERTY_SHOW_BUTTON_PANEL).toBool();

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

  wgtRLI->setGeometry(QRect(QPoint(0, 0), availableSize));
}

void MainWindow::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);
  wgtRLI->update();
}

void MainWindow::onRLIWidgetInitialized() {
  connect( _radar_ds, SIGNAL(updateData(uint,uint,GLfloat*))
         , wgtRLI->radarEngine(), SLOT(updateData(uint,uint,GLfloat*)));

  connect( _radar_ds, SIGNAL(updateData2(uint,uint,GLfloat*))
         , wgtRLI->tailsEngine(), SLOT(updateData(uint,uint,GLfloat*)));

  qRegisterMetaType<RadarTarget>("RadarTarget");
  connect(_target_ds, SIGNAL(updateTarget(QString, RadarTarget))
         , wgtRLI->targetEngine(), SLOT(updateTarget(QString, RadarTarget)));


  int frame = qApp->property(PROPERTY_FRAME_DELAY).toInt();
  startTimer(frame, Qt::CoarseTimer);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  pressedKeys.remove(event->key());
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  float chartScale;

  switch(event->key()) {
  case Qt::Key_PageUp:
    /*
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged(qMin(_gain_ctrl->value() + 5, 255));

    if (mod_keys & Qt::AltModifier)
      emit waterChanged(qMin(_water_ctrl->value() + 5, 255));

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged(qMin(_rain_ctrl->value() + 5, 255));
    */
    break;
  case Qt::Key_PageDown:
    /*
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged(qMin(_gain_ctrl->value() - 5, 255));

    if (mod_keys & Qt::AltModifier)
      emit waterChanged(qMin(_water_ctrl->value() - 5, 255));

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged(qMin(_rain_ctrl->value() - 5, 255));
    */
    break;

  // Под. имп. Помех
  case Qt::Key_S:
    break;
  // Меню
  case Qt::Key_W:
    if(pressedKeys.contains(Qt::Key_B))
       wgtRLI->onConfigMenuToggled();
     else
       wgtRLI->onMenuToggled();
     break;
  // Шкала +
  case Qt::Key_Plus:
    chartScale = RLIState::instance().chartScale();
    chartScale *= 0.95;
    RLIState::instance().onChartScaleChanged(chartScale);
    break;
  // Шкала -
  case Qt::Key_Minus:
    chartScale = RLIState::instance().chartScale();
    chartScale *= 1.05;
    RLIState::instance().onChartScaleChanged(chartScale);
    break;
  // Вынос центра
  case Qt::Key_C:
    break;
  // Скрытое меню
  case Qt::Key_U:
    break;
  // Следы точки
  case Qt::Key_T:
    break;
  // Выбор цели
  case Qt::Key_Up:
    wgtRLI->onUpToggled();
//    shipPos = RLIState::instance().shipPosition();
//    shipPos.first += 0.005;
//    RLIState::instance().onShipPositionChanged(shipPos);
    break;
  // ЛИД / ЛОД
  case Qt::Key_Down:
    wgtRLI->onDownToggled();
//    shipPos = RLIState::instance().shipPosition();
//    shipPos.first -= 0.005;
//    RLIState::instance().onShipPositionChanged(shipPos);
    break;
  case Qt::Key_Left:
//    shipPos = RLIState::instance().shipPosition();
//    shipPos.second -= 0.005;
//    RLIState::instance().onShipPositionChanged(shipPos);
    break;
  case Qt::Key_Right:
//    shipPos = RLIState::instance().shipPosition();
//    shipPos.second += 0.005;
//    RLIState::instance().onShipPositionChanged(shipPos);
    break;
  // Захват
  case Qt::Key_Enter:
    wgtRLI->onEnterToggled();
    break;
  // Захват
  case Qt::Key_Return:
    wgtRLI->onEnterToggled();
    break;
  //Сброс
  case Qt::Key_Escape:
    wgtRLI->onBackToggled();
    break;
  // Парал. Линии
  case Qt::Key_Backslash:
    wgtRLI->controlsEngine()->setParallelLinesVisible(!wgtRLI->controlsEngine()->isParallelLinesVisible());
    break;
  //Электронная лупа
  case Qt::Key_L:
    wgtRLI->onMagnifierToggled();
    break;
  //Обзор
  case Qt::Key_X:
    wgtRLI->toggleRadarTailsShift();
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
    wgtRLI->controlsEngine()->setCirclesVisible(!wgtRLI->controlsEngine()->isCirclesVisible());
    break;
  }

  QWidget::keyPressEvent(event);
  pressedKeys.insert(event->key());
}
