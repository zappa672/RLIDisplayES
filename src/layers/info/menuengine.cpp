#include "menuengine.h"

#include <QColor>

const QColor MENU_LOCKED_ITEM_COLOR   (0xB4, 0xB4, 0xB4);
const QColor MENU_DISABLED_ITEM_COLOR (0xFC, 0x54, 0x54);
const QColor MENU_TEXT_STATIC_COLOR   (0x00, 0xFC, 0xFC);
const QColor MENU_TEXT_DYNAMIC_COLOR  (0xFC, 0xFC, 0x54);

const QColor MENU_BORDER_COLOR        (0x40, 0xFC, 0x00);
const QColor MENU_BACKGRD_COLOR       (0x00, 0x00, 0x00);


RLIMenuItem::RLIMenuItem(char** name, QObject* parent) : QObject(parent) {
  _enabled = true;
  _locked = false;

  _enc = QTextCodec::codecForName("cp866")->makeEncoder();
  _dec = QTextCodec::codecForName("UTF8")->makeDecoder();

  for (int i = 0; i < RLI_LANG_COUNT; i++) {
    _name[i] = _enc->fromUnicode(_dec->toUnicode(name[i]));
  }
}


RLIMenuItemMenu::RLIMenuItemMenu(char** name, RLIMenuItemMenu* parent) : RLIMenuItem(name, parent) {
  _type = MENU;
  _parent = parent;
}



RLIMenuItemMenu::~RLIMenuItemMenu() {
  qDeleteAll(_items);
}



RLIMenuItemAction::RLIMenuItemAction(char** name, QObject* parent) : RLIMenuItem(name, parent) {
  _type = ACTION;
}

void RLIMenuItemAction::action() {
  emit triggered();
}



RLIMenuItemList::RLIMenuItemList(char** name, int def_ind, QObject* parent)
  : RLIMenuItem(name, parent) {
  _type = LIST;
  _index = def_ind;
}

void RLIMenuItemList::addVariant(char** values) {
  for (int i = 0; i < RLI_LANG_COUNT; i++) {
    _variants[i] << _enc->fromUnicode(_dec->toUnicode(values[i]));
  }
}

void RLIMenuItemList::up() {
  if (_index < _variants[RLI_LANG_RUSSIAN].size() - 1) {
    _index++;
    emit valueChanged(_variants[RLI_LANG_RUSSIAN][_index]);
  }
}

void RLIMenuItemList::down() {
  if (_index > 0) {
    _index--;
    emit valueChanged(_variants[RLI_LANG_RUSSIAN][_index]);
  }
}



RLIMenuItemInt::RLIMenuItemInt(char** name, int min, int max, int def, QObject* parent)
  : RLIMenuItem(name, parent) {
  _type = INT;

  _min = min;
  _max = max;
  _value = def;

  _change_start_time = QDateTime::currentDateTime().addSecs(-1);
  _last_change_time = QDateTime::currentDateTime().addSecs(-1);

  _delta=1;
}

void RLIMenuItemInt::up() {
  if (_value < _max) {
    adjustDelta();

    _value += _delta;

    if (_value > _max)
      _value = _max;

    emit valueChanged(_value);
  }
}

void RLIMenuItemInt::down() {
  if (_value > _min) {
    adjustDelta();

    _value -= _delta;

    if (_value < _min)
      _value = _min;

    emit valueChanged(_value);
  }
}

int RLIMenuItemInt::setValue(int val) {
  if((val < _min) || (val > _max))
    return -2;

  _value = val;
  return 0;
}

int RLIMenuItemInt::setValue(QByteArray val) {
  int  res;
  bool ok = false;

  res = val.toInt(&ok);
  if(!ok)
      return -1;
  if((res < _min) || (res > _max))
      return -2;
  _value = res;

  return 0;
}

void RLIMenuItemInt::adjustDelta() {
  QDateTime currentTime = QDateTime::currentDateTime();

  if (currentTime > _last_change_time.addSecs(1)) {
    _last_change_time = currentTime;
    _change_start_time = currentTime;
    _delta = 1;
  } else {
    if (_change_start_time.msecsTo(currentTime) > 5000) {
      _delta = 100;
    } else if (_change_start_time.msecsTo(currentTime) > 3000) {
      _delta = 10;
    } else {
      _delta = 1;
    }

    _last_change_time = currentTime;
  }
}




RLIMenuItemFloat::RLIMenuItemFloat(char** name, float min, float max, float def) : RLIMenuItem(name) {
  _type = FLOAT;
  _min = min;
  _max = max;
  _value = def;
  _step = 0.2f;
}






MenuEngine::MenuEngine(const RLIPanelInfo& params, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context)  {

  initializeOpenGLFunctions();

  _selected_line = 1;
  _selection_active = false;
  _state = DISABLED;

  _prog = new QOpenGLShaderProgram();
  _fbo = nullptr;

  _lang = RLI_LANG_RUSSIAN;

  _enc = QTextCodec::codecForName("cp866")->makeEncoder();
  _dec = QTextCodec::codecForName("UTF8")->makeDecoder();
  _dec1 = QTextCodec::codecForName("cp866")->makeDecoder();

  resize(params.size, params.position, params.params["font"]);

  glGenBuffers(INFO_ATTR_COUNT, _vbo_ids);
  initShader();

  initMainMenuTree();
  initCnfgMenuTree();

  _menu = NULL;
  //_routeEngine = NULL;
  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::initCnfgMenuTree() {
  RLIMenuItemMenu* m1 = new RLIMenuItemMenu(RLIStrings::nMenu1, NULL);

  // --------------------------
  RLIMenuItemMenu* m10 = new RLIMenuItemMenu(RLIStrings::nMenu10, m1);
  m1->add_item(m10);

  RLIMenuItemList* i100 = new RLIMenuItemList(RLIStrings::nMenu100, 4);
  i100->addVariant(RLIStrings::logSignal[0]);
  i100->addVariant(RLIStrings::logSignal[1]);
  i100->addVariant(RLIStrings::logSignal[2]);
  i100->addVariant(RLIStrings::logSignal[3]);
  i100->addVariant(RLIStrings::logSignal[4]);
  i100->addVariant(RLIStrings::logSignal[5]);
  i100->addVariant(RLIStrings::logSignal[6]);
  i100->addVariant(RLIStrings::logSignal[7]);
  m10->add_item(static_cast<RLIMenuItem*>(i100));

  RLIMenuItemInt* i101 = new RLIMenuItemInt(RLIStrings::nMenu101, 0, 255, 0);
  m10->add_item(static_cast<RLIMenuItem*>(i101));

  RLIMenuItemFloat* i102 = new RLIMenuItemFloat(RLIStrings::nMenu102, -179.9f, 179.9f, 0.f);
  m10->add_item(static_cast<RLIMenuItem*>(i102));

  RLIMenuItemFloat* i103 = new RLIMenuItemFloat(RLIStrings::nMenu103, 0.f, 359.9f, 0.f);
  m10->add_item(static_cast<RLIMenuItem*>(i103));

  RLIMenuItemFloat* i104 = new RLIMenuItemFloat(RLIStrings::nMenu104, 0.f, 359.9f, 0.f);
  m10->add_item(static_cast<RLIMenuItem*>(i104));

  RLIMenuItemList* i105 = new RLIMenuItemList(RLIStrings::nMenu105, 0);
  i105->addVariant(RLIStrings::bandArray[0]);
  i105->addVariant(RLIStrings::bandArray[1]);
  i105->addVariant(RLIStrings::bandArray[2]);
  m10->add_item(static_cast<RLIMenuItem*>(i105));
  connect(i105, SIGNAL(valueChanged(QByteArray)), this, SIGNAL(bandModeChanged(QByteArray)), Qt::QueuedConnection);

  RLIMenuItemList* i106 = new RLIMenuItemList(RLIStrings::nMenu106, 0);
  i106->addVariant(RLIStrings::OffOnArray[0]);
  i106->addVariant(RLIStrings::OffOnArray[1]);
  m10->add_item(static_cast<RLIMenuItem*>(i106));

  RLIMenuItemInt* i107 = new RLIMenuItemInt(RLIStrings::nMenu107, 25, 100, 80);
  m10->add_item(static_cast<RLIMenuItem*>(i107));


  // --------------------------
  RLIMenuItemMenu* m11 = new RLIMenuItemMenu(RLIStrings::nMenu11, m1);
  m1->add_item(m11);

  RLIMenuItemFloat* i110 = new RLIMenuItemFloat(RLIStrings::nMenu110, -5.f, 5.f, -4.4f);
  m11->add_item(static_cast<RLIMenuItem*>(i110));

  RLIMenuItemInt* i111 = new RLIMenuItemInt(RLIStrings::nMenu111, 1, 255, 170);
  m11->add_item(static_cast<RLIMenuItem*>(i111));

  RLIMenuItemInt* i112 = new RLIMenuItemInt(RLIStrings::nMenu112, -2048, 2048, 0);
  m11->add_item(static_cast<RLIMenuItem*>(i112));
  connect(i112, SIGNAL(valueChanged(int)), this, SIGNAL(analogZeroChanged(int)), Qt::QueuedConnection);
  analogZeroItem = i112;

  RLIMenuItemInt* i113 = new RLIMenuItemInt(RLIStrings::nMenu113, 1, 255, 25);
  m11->add_item(static_cast<RLIMenuItem*>(i113));

  RLIMenuItemInt* i114 = new RLIMenuItemInt(RLIStrings::nMenu114, 0, 255, 18);
  m11->add_item(static_cast<RLIMenuItem*>(i114));

  RLIMenuItemInt* i115 = new RLIMenuItemInt(RLIStrings::nMenu115, 0, 255, 16);
  m11->add_item(static_cast<RLIMenuItem*>(i115));

  RLIMenuItemInt* i116 = new RLIMenuItemInt(RLIStrings::nMenu116, 0, 4096, 100);
  m11->add_item(static_cast<RLIMenuItem*>(i116));

  RLIMenuItemInt* i117 = new RLIMenuItemInt(RLIStrings::nMenu117, 0, 255, 30);
  m11->add_item(static_cast<RLIMenuItem*>(i117));

  RLIMenuItemInt* i118 = new RLIMenuItemInt(RLIStrings::nMenu118, -20, 20, -2);
  m11->add_item(static_cast<RLIMenuItem*>(i118));

  RLIMenuItemInt* i119 = new RLIMenuItemInt(RLIStrings::nMenu119, -20, 20, -2);
  m11->add_item(static_cast<RLIMenuItem*>(i119));

  RLIMenuItemInt* i1110 = new RLIMenuItemInt(RLIStrings::nMenu1110, -20, 20, 0);
  m11->add_item(static_cast<RLIMenuItem*>(i1110));

  // --------------------------
  RLIMenuItemMenu* m12 = new RLIMenuItemMenu(RLIStrings::nMenu12, m1);
  m1->add_item(m12);

  RLIMenuItemList* i120 = new RLIMenuItemList(RLIStrings::nMenu120, 0);
  i120->addVariant(RLIStrings::OffOnArray[0]);
  i120->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i120));

  RLIMenuItemList* i121 = new RLIMenuItemList(RLIStrings::nMenu121, 0);
  i121->addVariant(RLIStrings::OffOnArray[0]);
  i121->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i121));

  RLIMenuItemList* i122 = new RLIMenuItemList(RLIStrings::nMenu122, 0);
  i122->addVariant(RLIStrings::OffOnArray[0]);
  i122->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i122));

  RLIMenuItemList* i123 = new RLIMenuItemList(RLIStrings::nMenu123, 0);
  i123->addVariant(RLIStrings::OffOnArray[0]);
  i123->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i123));

  RLIMenuItemList* i124 = new RLIMenuItemList(RLIStrings::nMenu124, 0);
  i124->addVariant(RLIStrings::OffOnArray[0]);
  i124->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i124));

  RLIMenuItemList* i125 = new RLIMenuItemList(RLIStrings::nMenu125, 0);
  i125->addVariant(RLIStrings::OffOnArray[0]);
  i125->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i125));

  RLIMenuItemList* i126 = new RLIMenuItemList(RLIStrings::nMenu126, 0);
  i126->addVariant(RLIStrings::OffOnArray[0]);
  i126->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i126));

  RLIMenuItemList* i127 = new RLIMenuItemList(RLIStrings::nMenu127, 0);
  i127->addVariant(RLIStrings::OffOnArray[0]);
  i127->addVariant(RLIStrings::OffOnArray[1]);
  m12->add_item(static_cast<RLIMenuItem*>(i127));

  // --------------------------
  RLIMenuItemMenu* m13 = new RLIMenuItemMenu(RLIStrings::nMenu13, m1);
  m1->add_item(m13);

  RLIMenuItemList* i130 = new RLIMenuItemList(RLIStrings::nMenu130, 1);
  i130->addVariant(RLIStrings::OffOnArray[0]);
  i130->addVariant(RLIStrings::OffOnArray[1]);
  m13->add_item(static_cast<RLIMenuItem*>(i130));

  RLIMenuItemList* i131 = new RLIMenuItemList(RLIStrings::nMenu131, 1);
  i131->addVariant(RLIStrings::OffOnArray[0]);
  i131->addVariant(RLIStrings::OffOnArray[1]);
  m13->add_item(static_cast<RLIMenuItem*>(i131));

  RLIMenuItemList* i132 = new RLIMenuItemList(RLIStrings::nMenu132, 1);
  i132->addVariant(RLIStrings::OffOnArray[0]);
  i132->addVariant(RLIStrings::OffOnArray[1]);
  m13->add_item(static_cast<RLIMenuItem*>(i132));

  RLIMenuItemInt* i133 = new RLIMenuItemInt(RLIStrings::nMenu133, 4800, 38400, 4800);
  m11->add_item(static_cast<RLIMenuItem*>(i133));


  // --------------------------
  _cnfg_menu = m1;
}


void MenuEngine::initMainMenuTree() {
  RLIMenuItemMenu* m0 = new RLIMenuItemMenu(RLIStrings::nMenu0, NULL);

  // --------------------------
  RLIMenuItemMenu* m00 = new RLIMenuItemMenu(RLIStrings::nMenu00, m0);
  m0->add_item(m00);

  RLIMenuItemInt* i000 = new RLIMenuItemInt(RLIStrings::nMenu000, 0, 255, 255);
  connect(i000, SIGNAL(valueChanged(int)), this, SIGNAL(radarBrightnessChanged(int)));
  m00->add_item(static_cast<RLIMenuItem*>(i000));

  RLIMenuItemInt* i001 = new RLIMenuItemInt(RLIStrings::nMenu001, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i001));

  RLIMenuItemInt* i002 = new RLIMenuItemInt(RLIStrings::nMenu002, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i002));

  RLIMenuItemInt* i003 = new RLIMenuItemInt(RLIStrings::nMenu003, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i003));

  RLIMenuItemInt* i004 = new RLIMenuItemInt(RLIStrings::nMenu004, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i004));

  RLIMenuItemInt* i005 = new RLIMenuItemInt(RLIStrings::nMenu005, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i005));

  RLIMenuItemInt* i006 = new RLIMenuItemInt(RLIStrings::nMenu006, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i006));

  RLIMenuItemInt* i007 = new RLIMenuItemInt(RLIStrings::nMenu007, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i007));

  RLIMenuItemList* i008 = new RLIMenuItemList(RLIStrings::nMenu008, 0);
  i008->addVariant(RLIStrings::dayArray[0]);
  i008->addVariant(RLIStrings::dayArray[1]);
  m00->add_item(static_cast<RLIMenuItem*>(i008));


  // --------------------------
  RLIMenuItemMenu* m01 = new RLIMenuItemMenu(RLIStrings::nMenu01, m0);
  m0->add_item(m01);

  RLIMenuItemFloat* i010 = new RLIMenuItemFloat(RLIStrings::nMenu010, 0.01f, 8.f, 2.f);
  m01->add_item(static_cast<RLIMenuItem*>(i010));

  RLIMenuItemInt* i011 = new RLIMenuItemInt(RLIStrings::nMenu011, 5, 60, 30);
  m01->add_item(static_cast<RLIMenuItem*>(i011));

  RLIMenuItemInt* i012 = new RLIMenuItemInt(RLIStrings::nMenu012, 5, 60, 30);
  m01->add_item(static_cast<RLIMenuItem*>(i012));

  RLIMenuItemList* i013 = new RLIMenuItemList(RLIStrings::nMenu013, 0);
  i013->addVariant(RLIStrings::trackArray[0]);
  i013->addVariant(RLIStrings::trackArray[1]);
  i013->addVariant(RLIStrings::trackArray[2]);
  i013->addVariant(RLIStrings::trackArray[3]);
  i013->addVariant(RLIStrings::trackArray[4]);
  m01->add_item(static_cast<RLIMenuItem*>(i013));
  connect(i013, SIGNAL(valueChanged(QByteArray)), this, SIGNAL(tailsModeChanged(QByteArray)), Qt::QueuedConnection);

  RLIMenuItemList* i014 = new RLIMenuItemList(RLIStrings::nMenu014, 1);
  i014->addVariant(RLIStrings::OffOnArray[0]);
  i014->addVariant(RLIStrings::OffOnArray[1]);
  m01->add_item(static_cast<RLIMenuItem*>(i014));

  RLIMenuItemAction* i015 = new RLIMenuItemAction(RLIStrings::nMenu015);
  i015->setLocked(true);
  m01->add_item(static_cast<RLIMenuItem*>(i015));

  RLIMenuItemAction* i016 = new RLIMenuItemAction(RLIStrings::nMenu016);
  i016->setLocked(true);
  m01->add_item(static_cast<RLIMenuItem*>(i016));

  RLIMenuItemList* i017 = new RLIMenuItemList(RLIStrings::nMenu017, 0);
  i017->addVariant(RLIStrings::tvecApArray[0]);
  i017->addVariant(RLIStrings::tvecApArray[1]);
  i017->setLocked(true);
  m01->add_item(static_cast<RLIMenuItem*>(i017));


  // --------------------------
  RLIMenuItemMenu* m02 = new RLIMenuItemMenu(RLIStrings::nMenu02, m0);
  m0->add_item(m02);

  RLIMenuItemInt* i020 = new RLIMenuItemInt(RLIStrings::nMenu020, 0, 255, 5);
  m02->add_item(static_cast<RLIMenuItem*>(i020));

  RLIMenuItemList* i021 = new RLIMenuItemList(RLIStrings::nMenu021, 0);
  i021->addVariant(RLIStrings::vdArray[0]);
  i021->addVariant(RLIStrings::vdArray[1]);
  m02->add_item(static_cast<RLIMenuItem*>(i021));

  RLIMenuItemList* i022 = new RLIMenuItemList(RLIStrings::nMenu022, 0);
  i022->addVariant(RLIStrings::speedArray[0]);
  i022->addVariant(RLIStrings::speedArray[1]);
  m02->add_item(static_cast<RLIMenuItem*>(i022));

  RLIMenuItemFloat* i023 = new RLIMenuItemFloat(RLIStrings::nMenu023, 0.f, 90.f, 5.f);
  m02->add_item(i023);

  RLIMenuItemList* i024 = new RLIMenuItemList(RLIStrings::nMenu024, 2);
  i024->addVariant(RLIStrings::devStabArray[0]);
  i024->addVariant(RLIStrings::devStabArray[1]);
  i024->addVariant(RLIStrings::devStabArray[2]);
  i024->addVariant(RLIStrings::devStabArray[3]);
  m02->add_item(i024);

  RLIMenuItemInt* i025 = new RLIMenuItemInt(RLIStrings::nMenu025, 0, 90, 0);
  m02->add_item(i025);

  RLIMenuItemList* i026 = new RLIMenuItemList(RLIStrings::nMenu026, 1);
  i026->addVariant(RLIStrings::langArray[0]);
  i026->addVariant(RLIStrings::langArray[1]);
  connect(i026, SIGNAL(valueChanged(QByteArray)), this, SIGNAL(languageChanged(QByteArray)), Qt::QueuedConnection);
  m02->add_item(i026);

  RLIMenuItemFloat* i027 = new RLIMenuItemFloat(RLIStrings::nMenu027, 0.f, 359.9f, 0.f);
  m02->add_item(i027);

  RLIMenuItemInt* i028 = new RLIMenuItemInt(RLIStrings::nMenu028, 1, 100, 1);
  m02->add_item(i028);

  RLIMenuItemInt* i029 = new RLIMenuItemInt(RLIStrings::nMenu029, 1, 100, 1);
  i029->setLocked(true);
  m02->add_item(i029);

  RLIMenuItemList* i02A = new RLIMenuItemList(RLIStrings::nMenu02A, 0);
  i02A->addVariant(RLIStrings::OffOnArray[0]);
  i02A->addVariant(RLIStrings::OffOnArray[1]);
  m02->add_item(i02A);


  // --------------------------
  RLIMenuItemMenu* m03 = new RLIMenuItemMenu(RLIStrings::nMenu03, m0);
  m0->add_item(m03);

  RLIMenuItemInt* i030 = new RLIMenuItemInt(RLIStrings::nMenu030, 1, 2, 1);
  m03->add_item(static_cast<RLIMenuItem*>(i030));

  RLIMenuItemList* i031 = new RLIMenuItemList(RLIStrings::nMenu031, 0);
  i031->addVariant(RLIStrings::OffOnArray[0]);
  i031->addVariant(RLIStrings::OffOnArray[1]);
  connect(i031, SIGNAL(valueChanged(const QByteArray)), this, SIGNAL(simulationChanged(QByteArray)), Qt::QueuedConnection);
  m03->add_item(static_cast<RLIMenuItem*>(i031));

  RLIMenuItemList* i032 = new RLIMenuItemList(RLIStrings::nMenu032, 1);
  i032->addVariant(RLIStrings::OffOnArray[0]);
  i032->addVariant(RLIStrings::OffOnArray[1]);
  m03->add_item(static_cast<RLIMenuItem*>(i032));

  RLIMenuItemList* i033 = new RLIMenuItemList(RLIStrings::nMenu033, 1);
  i033->addVariant(RLIStrings::OffOnArray[0]);
  i033->addVariant(RLIStrings::OffOnArray[1]);
  m03->add_item(static_cast<RLIMenuItem*>(i033));

  RLIMenuItemFloat* i034 = new RLIMenuItemFloat(RLIStrings::nMenu034, 0.f, 3.f, 0.f);
  m03->add_item(static_cast<RLIMenuItem*>(i034));

  RLIMenuItemList* i035 = new RLIMenuItemList(RLIStrings::nMenu035, 0);
  i035->addVariant(RLIStrings::YesNoArray[0]);
  i035->addVariant(RLIStrings::YesNoArray[1]);
  m03->add_item(static_cast<RLIMenuItem*>(i035));

  RLIMenuItemList* i036 = new RLIMenuItemList(RLIStrings::nMenu036, 1);
  i036->addVariant(RLIStrings::YesNoArray[0]);
  i036->addVariant(RLIStrings::YesNoArray[1]);
  i036->setEnabled(false);
  m03->add_item(static_cast<RLIMenuItem*>(i036));

  RLIMenuItemList* i037 = new RLIMenuItemList(RLIStrings::nMenu037, 1);
  i037->addVariant(RLIStrings::YesNoArray[0]);
  i037->addVariant(RLIStrings::YesNoArray[1]);
  i037->setEnabled(false);
  m03->add_item(static_cast<RLIMenuItem*>(i037));

  RLIMenuItemList* i038 = new RLIMenuItemList(RLIStrings::nMenu038, 1);
  i038->addVariant(RLIStrings::YesNoArray[0]);
  i038->addVariant(RLIStrings::YesNoArray[1]);
  i038->setEnabled(false);
  m03->add_item(static_cast<RLIMenuItem*>(i038));


  // --------------------------
  RLIMenuItemMenu* m04 = new RLIMenuItemMenu(RLIStrings::nMenu04, m0);
  m0->add_item(m04);

  RLIMenuItemInt* i040 = new RLIMenuItemInt(RLIStrings::nMenu040, 1, 4, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i040));
  routeLoaderItem = i040;

  RLIMenuItemInt* i041 = new RLIMenuItemInt(RLIStrings::nMenu041, 0, 10, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i041));

  RLIMenuItemAction* i042 = new RLIMenuItemAction(RLIStrings::nMenu042);
  i042->setLocked(true);
  m04->add_item(static_cast<RLIMenuItem*>(i042));

  RLIMenuItemInt* i043 = new RLIMenuItemInt(RLIStrings::nMenu043, 40, 1000, 200);
  m04->add_item(static_cast<RLIMenuItem*>(i043));

  RLIMenuItemAction* i044 = new RLIMenuItemAction(RLIStrings::nMenu044);
  m04->add_item(static_cast<RLIMenuItem*>(i044));
  routeEditItem = i044;

  RLIMenuItemInt* i045 = new RLIMenuItemInt(RLIStrings::nMenu045, 0, 10, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i045));

  RLIMenuItemList* i046 = new RLIMenuItemList(RLIStrings::nMenu046, 0);
  i046->addVariant(RLIStrings::nameSymb[0]);
  i046->addVariant(RLIStrings::nameSymb[1]);
  i046->addVariant(RLIStrings::nameSymb[2]);
  i046->addVariant(RLIStrings::nameSymb[3]);
  i046->addVariant(RLIStrings::nameSymb[4]);
  m04->add_item(static_cast<RLIMenuItem*>(i046));

  RLIMenuItemInt* i047 = new RLIMenuItemInt(RLIStrings::nMenu047, 1, 4, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i047));
  routeSaverItem = i047;


  // --------------------------
  RLIMenuItemMenu* m05 = new RLIMenuItemMenu(RLIStrings::nMenu05, m0);
  m0->add_item(m05);

  RLIMenuItemList* i050 = new RLIMenuItemList(RLIStrings::nMenu050, 0);
  i050->addVariant(RLIStrings::nameSign[0]);
  i050->addVariant(RLIStrings::nameSign[1]);
  i050->addVariant(RLIStrings::nameSign[2]);
  m05->add_item(static_cast<RLIMenuItem*>(i050));

  RLIMenuItemList* i051 = new RLIMenuItemList(RLIStrings::nMenu051, 0);
  i051->addVariant(RLIStrings::nameRecog[0]);
  i051->addVariant(RLIStrings::nameRecog[1]);
  i051->addVariant(RLIStrings::nameRecog[2]);
  m05->add_item(static_cast<RLIMenuItem*>(i051));

  RLIMenuItemList* i052 = new RLIMenuItemList(RLIStrings::nMenu052, 0);
  i052->addVariant(RLIStrings::OffOnArray[0]);
  i052->addVariant(RLIStrings::OffOnArray[1]);
  m05->add_item(static_cast<RLIMenuItem*>(i052));

  // --------------------------
  _main_menu = m0;
}

MenuEngine::~MenuEngine() {
  delete _prog;
  delete _fbo;
  glDeleteBuffers(INFO_ATTR_COUNT, _vbo_ids);
  delete _main_menu;
}

void MenuEngine::setState(MenuState state) {
  _state = state;
  switch (_state) {
  case DISABLED:
      _selection_active = false;
      _menu = NULL;
      break;
  case MAIN:
      _menu = _main_menu;
      break;
  case CONFIG:
      _menu = _cnfg_menu;
      break;
  }
  _selected_line = 1;
  _need_update = true;
  _last_action_time = QDateTime::currentDateTime();
}

void MenuEngine::onAnalogZeroChanged(int val) {
  analogZeroItem->setValue(val);
}

void MenuEngine::onLanguageChanged(const QByteArray& lang) {
  QString lang_str = _dec1->toUnicode(lang);

  if (_lang == RLI_LANG_RUSSIAN && (lang_str == _dec->toUnicode(RLIStrings::nEng[RLI_LANG_RUSSIAN])
                             || lang_str == _dec->toUnicode(RLIStrings::nEng[RLI_LANG_ENGLISH]))) {
      _lang = RLI_LANG_ENGLISH;
      _need_update = true;
  }

  if (_lang == RLI_LANG_ENGLISH && (lang_str == _dec->toUnicode(RLIStrings::nRus[RLI_LANG_ENGLISH])
                             || lang_str == _dec->toUnicode(RLIStrings::nRus[RLI_LANG_RUSSIAN]))) {
      _lang = RLI_LANG_RUSSIAN;
      _need_update = true;
  }
}

void MenuEngine::onUp() {
  if (_menu == NULL)
    return;

  if (!_selection_active) {
    if (_selected_line > 1)
      _selected_line--;
  } else {
    _menu->item(_selected_line - 1)->up();
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onDown() {
  if (_menu == NULL)
    return;

  if (!_selection_active) {
    if (_selected_line < _menu->item_count())
      _selected_line++;
  } else {
    _menu->item(_selected_line - 1)->down();
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onEnter() {
  if ( (_menu->item(_selected_line-1)->locked()
        || !_menu->item(_selected_line-1)->enabled() )
       && !_selection_active)
    return;

  if (_menu->item(_selected_line - 1)->type() == RLIMenuItem::MENU) {
    _menu = dynamic_cast<RLIMenuItemMenu*>(_menu->item(_selected_line - 1));
    _selected_line = 1;
  } else {
    _selection_active = !_selection_active;

    if (_menu->item(_selected_line - 1) == routeEditItem) {
      if (_selection_active)
        emit startRouteEdit();
      else
        emit finishRouteEdit();
    }

    if (!_selection_active) {
      if (_menu->item(_selected_line - 1) == routeLoaderItem)
        emit loadRoute(routeLoaderItem->intValue());

      if (_menu->item(_selected_line - 1) == routeSaverItem)
        emit saveRoute(routeSaverItem->intValue());

    }
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onBack() {
  if (!_selection_active && _menu->parent() != NULL) {
     int _new_selection = 1;
     for (int i = 0; i < _menu->parent()->item_count(); i++)
       if (_menu->parent()->item(i) == _menu)
         _new_selection = i+1;

     _selected_line = _new_selection;
     _menu = _menu->parent();

     _last_action_time = QDateTime::currentDateTime();
     _need_update = true;
  }
}


void MenuEngine::resize(const QSize& sz, const QPoint &pos, const QString& font) {
  _size = sz;
  _position = pos;
  _font_tag = font;

  if (_fbo != nullptr)
    delete _fbo;

  _fbo = new QOpenGLFramebufferObject(_size);

  _need_update = true;
}

void MenuEngine::update() {
  if (visible() && _last_action_time.secsTo(QDateTime::currentDateTime()) > 90)
    setState(DISABLED);

  if (!_need_update)
    return;

  glViewport(0, 0, _size.width(), _size.height());

  glEnable(GL_BLEND);

  _fbo->bind();

  glClearColor(MENU_BACKGRD_COLOR.redF(), MENU_BACKGRD_COLOR.greenF(), MENU_BACKGRD_COLOR.blueF(), 1.f);
  glClear(GL_COLOR_BUFFER_BIT);


  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _size.width(), 0.f, _size.height(), -1.f, 1.f);

  _prog->bind();

  _prog->setUniformValue(_uniform_locs[INFO_UNIF_MVP], projection);

  // Border
  drawRect(QRect(QPoint(0, 0), QSize(_size.width(), 1)), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(0, 0), QSize(1, _size.height())), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(0, _size.height()-1), QSize(_size.width(), 1)), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(_size.width()-1, 0), QSize(1, _size.height())), MENU_BORDER_COLOR);

  if (_menu != NULL) {
    QSize font_size = _fonts->getFontSize(_font_tag);

    // Header separator
    drawRect(QRect(QPoint(0, font_size.height() + 6), QSize(_size.width(), 1)), MENU_BORDER_COLOR);
    // Footer separator
    drawRect(QRect(QPoint(0, _size.height() - font_size.height() - 6), QSize(_size.width(), 1)), MENU_BORDER_COLOR);

    // Header
    drawText(_menu->name(_lang), 0, ALIGN_CENTER, MENU_TEXT_STATIC_COLOR);

    // Menu
    for (int i = 0; i < _menu->item_count(); i++) {
//      if ((_menu->item(i) == routeLoaderItem || _menu->item(i) == routeSaverItem) && _routeEngine != NULL) {
//        if (_routeEngine->isIndexUsed(static_cast<RLIMenuItemInt*>(_menu->item(i))->intValue())) {
//          drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_TEXT_STATIC_COLOR);
//          drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_TEXT_DYNAMIC_COLOR);
//        } else {
//          drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_TEXT_STATIC_COLOR);
//          drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_DISABLED_ITEM_COLOR);
//        }

//        continue;
//      }

      if (_menu->item(i)->locked()) {
        drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_LOCKED_ITEM_COLOR);
        drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_LOCKED_ITEM_COLOR);
      } else if (_menu->item(i)->enabled()) {
        drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_TEXT_STATIC_COLOR);
        drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_TEXT_DYNAMIC_COLOR);
      } else {
        drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_LOCKED_ITEM_COLOR);
        drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_DISABLED_ITEM_COLOR);
      }
    }

    drawBar();
    drawSelection();
  }

  _prog->release();
  _fbo->release();

  _need_update = false;
}

void MenuEngine::drawSelection() {
  QSize font_size = _fonts->getFontSize(_font_tag);

  QColor col;
  if (_selection_active)
    col = MENU_TEXT_DYNAMIC_COLOR;
  else
    col = MENU_TEXT_STATIC_COLOR;

  QPoint anchor = QPoint(2, 2+_selected_line*(6+font_size.height()));
  QSize  size = QSize(_size.width() - 4, font_size.height() + 4);

  drawRect(QRect(anchor, QSize(size.width(), 1)), col);
  drawRect(QRect(anchor, QSize(1, size.height())), col);
  drawRect(QRect(anchor + QPoint(0, size.height() - 1), QSize(size.width(), 1)), col);
  drawRect(QRect(anchor + QPoint(size.width()-1, 0), QSize(1, size.height())), col);
}

void MenuEngine::drawBar() {
  if (_menu == NULL)
    return;

  QSize size = _fbo->size();
  int bar_width = 0;

  RLIMenuItem* currItem = _menu->item(_selected_line - 1);

  RLIMenuItemInt* intItem = dynamic_cast<RLIMenuItemInt*>(currItem);
  if (intItem != NULL) {
    int val = intItem->intValue();
    int min_val = intItem->minValue();
    int max_val = intItem->maxValue();
    bar_width = (size.width() - 2) * static_cast<float>(val - min_val) / (max_val - min_val);
  }

  RLIMenuItemFloat* fltItem = dynamic_cast<RLIMenuItemFloat*>(currItem);
  if (fltItem != NULL) {
    float val = fltItem->fltValue();
    float min_val = fltItem->minValue();
    float max_val = fltItem->maxValue();
    bar_width = (size.width() - 2) * (val - min_val) / (max_val - min_val);
  }

  drawRect(QRect(QPoint(1, size.height()-17), QSize(bar_width, 14)), MENU_BORDER_COLOR);
}

void MenuEngine::drawRect(const QRect& rect, const QColor& col) {
  std::vector<GLfloat> pos;

  pos.push_back(rect.x());
  pos.push_back(rect.y());
  pos.push_back(rect.x());
  pos.push_back(rect.y() + rect.height());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y() + rect.height());

  glUniform2f(_uniform_locs[INFO_UNIF_SIZE], 0.f, 0.f);
  glUniform4f(_uniform_locs[INFO_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glVertexAttrib1f(_attr_locs[INFO_ATTR_ORDER], 0.f);
  glDisableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glVertexAttrib1f(_attr_locs[INFO_ATTR_CHAR_VAL], 0.f);
  glDisableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void MenuEngine::drawText(const QByteArray& text, int line, TextAllignement align, const QColor& col) {
  GLuint tex_id = _fonts->getTexture(_font_tag)->textureId();
  QSize font_size = _fonts->getFontSize(_font_tag);

  std::vector<GLfloat> pos;
  std::vector<GLfloat> ord, chars;

  QPoint anchor = QPoint(0, 4 + (font_size.height() + 6) * line);

  switch (align) {
    case ALIGN_CENTER:
      anchor.setX(_size.width()/2 - text.size()*font_size.width()/2);
      break;
    case ALIGN_LEFT:
      anchor.setX(4);
      break;
    case ALIGN_RIGHT:
      anchor.setX(_size.width() - 4 - text.size()*font_size.width());
      break;
    default:
      return;
  }

  // From text to vertex data
  for (int i = 0; i < text.size(); i++) {
    for (int j = 0; j < 4; j++) {
      QPoint lefttop = anchor + QPoint(i * font_size.width(), 0);
      pos.push_back(lefttop.x());
      pos.push_back(lefttop.y());
      ord.push_back(j);
      chars.push_back(text[i]);
    }
  }


  glUniform2f(_uniform_locs[INFO_UNIF_SIZE], font_size.width(), font_size.height());
  glUniform4f(_uniform_locs[INFO_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), 1.f);


  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, ord.size()*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, chars.size()*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, ord.size());

  glBindTexture(GL_TEXTURE_2D, 0);
}


void MenuEngine::initShader() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/info.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/info.frag.glsl");
  _prog->link();
  _prog->bind();

  _attr_locs[INFO_ATTR_POSITION]  = _prog->attributeLocation("position");
  _attr_locs[INFO_ATTR_ORDER]     = _prog->attributeLocation("order");
  _attr_locs[INFO_ATTR_CHAR_VAL]  = _prog->attributeLocation("char_val");

  _uniform_locs[INFO_UNIF_MVP]    = _prog->uniformLocation("mvp_matrix");
  _uniform_locs[INFO_UNIF_COLOR]  = _prog->uniformLocation("color");
  _uniform_locs[INFO_UNIF_SIZE]   = _prog->uniformLocation("size");

  _prog->release();
}
