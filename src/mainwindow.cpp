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

  _radar_ds = new RadarDataSource(this);
  _ship_ds = new ShipDataSource(this);
  _target_ds = new TargetDataSource(this);

  _radar_ds->start();
  _ship_ds->start();
  _target_ds->start();

  RLIState::instance().onShipPositionChanged(_ship_ds->getPosition());

  connect(_ship_ds, SIGNAL(positionChanged(std::pair<float,float>))
         ,&RLIState::instance(), SLOT(onShipPositionChanged(std::pair<float,float>)));

  _gain_ctrl = new ValueBarController(RLIStrings::nGain, 255, this);
  connect(this, SIGNAL(gainChanged(int)), _gain_ctrl, SLOT(onValueChanged(int)));
  connect(wgtButtonPanel, SIGNAL(gainChanged(int)), _gain_ctrl, SLOT(onValueChanged(int)));

  _water_ctrl = new ValueBarController(RLIStrings::nWave, 255, this);
  connect(this, SIGNAL(waterChanged(int)), _water_ctrl, SLOT(onValueChanged(int)));
  connect(wgtButtonPanel, SIGNAL(waterChanged(int)), _water_ctrl, SLOT(onValueChanged(int)));

  _rain_ctrl = new ValueBarController(RLIStrings::nRain, 255, this);
  connect(this, SIGNAL(rainChanged(int)), _rain_ctrl, SLOT(onValueChanged(int)));
  connect(wgtButtonPanel, SIGNAL(rainChanged(int)), _rain_ctrl, SLOT(onValueChanged(int)));

  _apch_ctrl = new ValueBarController(RLIStrings::nAfc, 255, this);
  _rdtn_ctrl = new ValueBarController(RLIStrings::nEmsn, 255, this);


  _lbl5_ctrl = new LabelController(RLIStrings::nPP12p, this);
  _band_lbl_ctrl = new LabelController(RLIStrings::nBandS, this);

  _lbl1_ctrl = new LabelController(RLIStrings::nNord, this);
  _lbl2_ctrl = new LabelController(RLIStrings::nRm, this);
  _lbl3_ctrl = new LabelController(RLIStrings::nWstab, this);
  _lbl4_ctrl = new LabelController(RLIStrings::nLod, this);

  _curs_ctrl = new CursorController(this);
  //connect(wgtRLI, SIGNAL(cursor_moved(float,float, const char *)), _curs_ctrl, SLOT(cursor_moved(float,float, const char *)));

  _clck_ctrl = new ClockController(this);
  connect(wgtRLI, SIGNAL(secondChanged()), _clck_ctrl, SLOT(onSecondChanged()));

  _scle_ctrl = new ScaleController(this);
  _fps_ctrl = new FpsController(this);

  _crse_ctrl = new CourseController(this);
  _pstn_ctrl = new PositionController(this);
  _blnk_ctrl = new BlankController(this);
  _dngr_ctrl = new DangerController(this);
  _tals_ctrl = new TailsController(this);
  _dgdt_ctrl = new DangerDetailsController(this);
  _vctr_ctrl = new VectorController(this);
  _trgs_ctrl = new TargetsController(this);

  _vn_ctrl = new VnController(this);
  _vd_ctrl = new VdController(this);
}

MainWindow::~MainWindow() {
  _radar_ds->finish();
  _ship_ds->finish();

  delete _radar_ds;
  delete _ship_ds;
  delete _target_ds;

  delete _gain_ctrl;
  delete _water_ctrl;
  delete _rain_ctrl;
  delete _apch_ctrl;
  delete _rdtn_ctrl;

  delete _scle_ctrl;

  delete _lbl1_ctrl;
  delete _lbl2_ctrl;
  delete _lbl3_ctrl;
  delete _lbl4_ctrl;
  delete _lbl5_ctrl;
  delete _band_lbl_ctrl;

  delete _crse_ctrl;

  delete _pstn_ctrl;
  delete _blnk_ctrl;
  delete _dngr_ctrl;

  delete _curs_ctrl;
  delete _clck_ctrl;
  delete _fps_ctrl;

  delete _tals_ctrl;
  delete _dgdt_ctrl;
  delete _vctr_ctrl;
  delete _trgs_ctrl;

  delete _vn_ctrl;
  delete _vd_ctrl;

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


  const RLILayout* layout = RLIConfig::instance().currentLayout();

  _gain_ctrl->resize(curSize, layout->panels["gain"]);
  _water_ctrl->resize(curSize, layout->panels["water"]);
  _rain_ctrl->resize(curSize, layout->panels["rain"]);
  _apch_ctrl->resize(curSize, layout->panels["apch"]);

  _rdtn_ctrl->resize(curSize, layout->panels["emission"]);
  _curs_ctrl->resize(curSize, layout->panels["cursor"]);
  _clck_ctrl->resize(curSize, layout->panels["clock"]);
  _fps_ctrl->resize(curSize, layout->panels["fps"]);
  _pstn_ctrl->resize(curSize, layout->panels["position"]);
  _blnk_ctrl->resize(curSize, layout->panels["blank"]);
  _crse_ctrl->resize(curSize, layout->panels["course"]);
  _scle_ctrl->resize(curSize, layout->panels["scale"]);

  _lbl1_ctrl->resize(curSize, layout->panels["label1"]);
  _lbl2_ctrl->resize(curSize, layout->panels["label2"]);
  _lbl3_ctrl->resize(curSize, layout->panels["label3"]);
  _lbl4_ctrl->resize(curSize, layout->panels["label4"]);
  _lbl5_ctrl->resize(curSize, layout->panels["label5"]);

  _band_lbl_ctrl->resize(curSize, layout->panels["band"]);
  _dngr_ctrl->resize(curSize, layout->panels["danger"]);

  _tals_ctrl->resize(curSize, layout->panels["tails"]);
  _dgdt_ctrl->resize(curSize, layout->panels["danger-details"]);
  _vctr_ctrl->resize(curSize, layout->panels["vector"]);
  _trgs_ctrl->resize(curSize, layout->panels["targets"]);

  _vn_ctrl->resize(curSize, layout->panels["vn"]);
  _vd_ctrl->resize(curSize, layout->panels["vd"]);
}

void MainWindow::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  _fps_ctrl->setFpsVal(wgtRLI->frameRate());

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


  const RLILayout* layout = RLIConfig::instance().currentLayout();

  setupInfoBlock(_gain_ctrl, layout->panels["gain"]);
  setupInfoBlock(_water_ctrl, layout->panels["water"]);
  setupInfoBlock(_rain_ctrl, layout->panels["rain"]);
  setupInfoBlock(_apch_ctrl, layout->panels["apch"]);

  setupInfoBlock(_rdtn_ctrl, layout->panels["emission"]);
  setupInfoBlock(_curs_ctrl, layout->panels["cursor"]);
  setupInfoBlock(_clck_ctrl, layout->panels["clock"]);
  setupInfoBlock(_fps_ctrl, layout->panels["fps"]);
  setupInfoBlock(_pstn_ctrl, layout->panels["position"]);
  setupInfoBlock(_blnk_ctrl, layout->panels["blank"]);
  setupInfoBlock(_crse_ctrl, layout->panels["course"]);
  setupInfoBlock(_scle_ctrl, layout->panels["scale"]);

  setupInfoBlock(_lbl1_ctrl, layout->panels["label1"]);
  setupInfoBlock(_lbl2_ctrl, layout->panels["label2"]);
  setupInfoBlock(_lbl3_ctrl, layout->panels["label3"]);
  setupInfoBlock(_lbl4_ctrl, layout->panels["label4"]);
  setupInfoBlock(_lbl5_ctrl, layout->panels["label5"]);

  setupInfoBlock(_band_lbl_ctrl, layout->panels["band"]);
  setupInfoBlock(_dngr_ctrl, layout->panels["danger"]);

  setupInfoBlock(_tals_ctrl, layout->panels["tails"]);
  setupInfoBlock(_dgdt_ctrl, layout->panels["danger-details"]);
  setupInfoBlock(_vctr_ctrl, layout->panels["vector"]);
  setupInfoBlock(_trgs_ctrl, layout->panels["targets"]);

  setupInfoBlock(_vn_ctrl, layout->panels["vn"]);
  setupInfoBlock(_vd_ctrl, layout->panels["vd"]);
}

void MainWindow::setupInfoBlock(InfoBlockController* ctrl, const RLIPanelInfo& panelInfo) {
  InfoBlock* blck = wgtRLI->infoEngine()->addInfoBlock();
  ctrl->setupBlock(blck, RLIConfig::instance().currentSize(), panelInfo);

  connect(ctrl, SIGNAL(setRect(int, QRect)), blck, SLOT(setRect(int, QRect)));
  connect(ctrl, SIGNAL(setText(int, int, QByteArray)), blck, SLOT(setText(int, int, QByteArray)));
}



void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  pressedKeys.remove(event->key());
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  std::pair<float, float> shipPos;
  Qt::KeyboardModifiers mod_keys = event->modifiers();

  float chartScale;

  switch(event->key()) {
  case Qt::Key_PageUp:
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged(qMin(_gain_ctrl->value() + 5, 255));

    if (mod_keys & Qt::AltModifier)
      emit waterChanged(qMin(_water_ctrl->value() + 5, 255));

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged(qMin(_rain_ctrl->value() + 5, 255));

    break;
  case Qt::Key_PageDown:
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged(qMin(_gain_ctrl->value() - 5, 255));

    if (mod_keys & Qt::AltModifier)
      emit waterChanged(qMin(_water_ctrl->value() - 5, 255));

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged(qMin(_rain_ctrl->value() - 5, 255));

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
