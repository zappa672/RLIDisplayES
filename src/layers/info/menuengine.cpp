#include "menuengine.h"
#include "../../common/properties.h"

#include <QColor>

const QColor MENU_LOCKED_ITEM_COLOR   (0xB4, 0xB4, 0xB4);
const QColor MENU_DISABLED_ITEM_COLOR (0xFC, 0x54, 0x54);
const QColor MENU_TEXT_STATIC_COLOR   (0x00, 0xFC, 0xFC);
const QColor MENU_TEXT_DYNAMIC_COLOR  (0xFC, 0xFC, 0x54);

const QColor MENU_BORDER_COLOR        (0x40, 0xFC, 0x00);
const QColor MENU_BACKGRD_COLOR       (0x00, 0x00, 0x00);


MenuEngine::MenuEngine(const RLIMenuLayout& layout, QOpenGLContext* context, QObject* parent)
  : QObject(parent), QOpenGLFunctions(context)  {

  initializeOpenGLFunctions();

  _selected_line = 1;
  _selection_active = false;

  _prog = new QOpenGLShaderProgram();
  _fbo = nullptr;

  _lang = RLI_LANG_RUSSIAN;

  resize(layout);

  glGenBuffers(INFO_ATTR_COUNT, _vbo_ids);
  initShader();

  initMainMenuTree();
  initCnfgMenuTree();

  _menu = nullptr;
  //_routeEngine = nullptr;
  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::initCnfgMenuTree() {
  RLIMenuItemMenu* m1 = new RLIMenuItemMenu(RLI_STR_MENU_1, nullptr);

  // --------------------------
  RLIMenuItemMenu* m10 = new RLIMenuItemMenu(RLI_STR_MENU_10, m1);
  m1->add_item(m10);

  RLIMenuItemList* i100 = new RLIMenuItemList(RLI_STR_MENU_100, 4);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_200_MINUS);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_500_MINUS);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_COD_MINUS);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_NMEA);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_COD_PLUS);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_500_PLUS);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_200_PLUS);
  i100->addVariant(RLI_STR_ARRAY_LOG_SIGNAL_GPS);
  m10->add_item(static_cast<RLIMenuItem*>(i100));

  RLIMenuItemInt* i101 = new RLIMenuItemInt(RLI_STR_MENU_101, 0, 255, 0);
  m10->add_item(static_cast<RLIMenuItem*>(i101));

  RLIMenuItemFloat* i102 = new RLIMenuItemFloat(RLI_STR_MENU_102, -179.9f, 179.9f, 0.f);
  m10->add_item(static_cast<RLIMenuItem*>(i102));

  RLIMenuItemFloat* i103 = new RLIMenuItemFloat(RLI_STR_MENU_103, 0.f, 359.9f, 0.f);
  m10->add_item(static_cast<RLIMenuItem*>(i103));

  RLIMenuItemFloat* i104 = new RLIMenuItemFloat(RLI_STR_MENU_104, 0.f, 359.9f, 0.f);
  m10->add_item(static_cast<RLIMenuItem*>(i104));

  RLIMenuItemList* i105 = new RLIMenuItemList(RLI_STR_MENU_105, 0);
  i105->addVariant(RLI_STR_ARRAY_BAND_X);
  i105->addVariant(RLI_STR_ARRAY_BAND_S);
  i105->addVariant(RLI_STR_ARRAY_BAND_K);
  m10->add_item(static_cast<RLIMenuItem*>(i105));
  connect(i105, SIGNAL(valueChanged(RLIString)), this, SIGNAL(bandModeChanged(RLIString)), Qt::QueuedConnection);

  RLIMenuItemList* i106 = new RLIMenuItemList(RLI_STR_MENU_106, 0);
  i106->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i106->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m10->add_item(static_cast<RLIMenuItem*>(i106));

  RLIMenuItemInt* i107 = new RLIMenuItemInt(RLI_STR_MENU_107, 25, 100, 80);
  m10->add_item(static_cast<RLIMenuItem*>(i107));


  // --------------------------
  RLIMenuItemMenu* m11 = new RLIMenuItemMenu(RLI_STR_MENU_11, m1);
  m1->add_item(m11);

  RLIMenuItemFloat* i110 = new RLIMenuItemFloat(RLI_STR_MENU_110, -5.f, 5.f, -4.4f);
  m11->add_item(static_cast<RLIMenuItem*>(i110));

  RLIMenuItemInt* i111 = new RLIMenuItemInt(RLI_STR_MENU_111, 1, 255, 170);
  m11->add_item(static_cast<RLIMenuItem*>(i111));

  RLIMenuItemInt* i112 = new RLIMenuItemInt(RLI_STR_MENU_112, -2048, 2048, 0);
  m11->add_item(static_cast<RLIMenuItem*>(i112));
  connect(i112, SIGNAL(valueChanged(int)), this, SIGNAL(analogZeroChanged(int)), Qt::QueuedConnection);
  analogZeroItem = i112;

  RLIMenuItemInt* i113 = new RLIMenuItemInt(RLI_STR_MENU_113, 1, 255, 25);
  m11->add_item(static_cast<RLIMenuItem*>(i113));

  RLIMenuItemInt* i114 = new RLIMenuItemInt(RLI_STR_MENU_114, 0, 255, 18);
  m11->add_item(static_cast<RLIMenuItem*>(i114));

  RLIMenuItemInt* i115 = new RLIMenuItemInt(RLI_STR_MENU_115, 0, 255, 16);
  m11->add_item(static_cast<RLIMenuItem*>(i115));

  RLIMenuItemInt* i116 = new RLIMenuItemInt(RLI_STR_MENU_116, 0, 4096, 100);
  m11->add_item(static_cast<RLIMenuItem*>(i116));

  RLIMenuItemInt* i117 = new RLIMenuItemInt(RLI_STR_MENU_117, 0, 255, 30);
  m11->add_item(static_cast<RLIMenuItem*>(i117));

  RLIMenuItemInt* i118 = new RLIMenuItemInt(RLI_STR_MENU_118, -20, 20, -2);
  m11->add_item(static_cast<RLIMenuItem*>(i118));

  RLIMenuItemInt* i119 = new RLIMenuItemInt(RLI_STR_MENU_119, -20, 20, -2);
  m11->add_item(static_cast<RLIMenuItem*>(i119));

  RLIMenuItemInt* i1110 = new RLIMenuItemInt(RLI_STR_MENU_11A, -20, 20, 0);
  m11->add_item(static_cast<RLIMenuItem*>(i1110));

  // --------------------------
  RLIMenuItemMenu* m12 = new RLIMenuItemMenu(RLI_STR_MENU_12, m1);
  m1->add_item(m12);

  RLIMenuItemList* i120 = new RLIMenuItemList(RLI_STR_MENU_120, 0);
  i120->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i120->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i120));

  RLIMenuItemList* i121 = new RLIMenuItemList(RLI_STR_MENU_121, 0);
  i121->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i121->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i121));

  RLIMenuItemList* i122 = new RLIMenuItemList(RLI_STR_MENU_122, 0);
  i122->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i122->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i122));

  RLIMenuItemList* i123 = new RLIMenuItemList(RLI_STR_MENU_123, 0);
  i123->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i123->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i123));

  RLIMenuItemList* i124 = new RLIMenuItemList(RLI_STR_MENU_124, 0);
  i124->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i124->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i124));

  RLIMenuItemList* i125 = new RLIMenuItemList(RLI_STR_MENU_125, 0);
  i125->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i125->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i125));

  RLIMenuItemList* i126 = new RLIMenuItemList(RLI_STR_MENU_126, 0);
  i126->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i126->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i126));

  RLIMenuItemList* i127 = new RLIMenuItemList(RLI_STR_MENU_127, 0);
  i127->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i127->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m12->add_item(static_cast<RLIMenuItem*>(i127));

  // --------------------------
  RLIMenuItemMenu* m13 = new RLIMenuItemMenu(RLI_STR_MENU_13, m1);
  m1->add_item(m13);

  RLIMenuItemList* i130 = new RLIMenuItemList(RLI_STR_MENU_130, 1);
  i130->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i130->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m13->add_item(static_cast<RLIMenuItem*>(i130));

  RLIMenuItemList* i131 = new RLIMenuItemList(RLI_STR_MENU_131, 1);
  i131->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i131->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m13->add_item(static_cast<RLIMenuItem*>(i131));

  RLIMenuItemList* i132 = new RLIMenuItemList(RLI_STR_MENU_132, 1);
  i132->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i132->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m13->add_item(static_cast<RLIMenuItem*>(i132));

  RLIMenuItemInt* i133 = new RLIMenuItemInt(RLI_STR_MENU_133, 4800, 38400, 4800);
  m11->add_item(static_cast<RLIMenuItem*>(i133));


  // --------------------------
  _cnfg_menu = m1;
}


void MenuEngine::initMainMenuTree() {
  RLIMenuItemMenu* m0 = new RLIMenuItemMenu(RLI_STR_MENU_0, nullptr);

  // --------------------------
  RLIMenuItemMenu* m00 = new RLIMenuItemMenu(RLI_STR_MENU_00, m0);
  m0->add_item(m00);

  RLIMenuItemInt* i000 = new RLIMenuItemInt(RLI_STR_MENU_000, 0, 255, 255);
  connect(i000, SIGNAL(valueChanged(int)), this, SIGNAL(radarBrightnessChanged(int)));
  m00->add_item(static_cast<RLIMenuItem*>(i000));

  RLIMenuItemInt* i001 = new RLIMenuItemInt(RLI_STR_MENU_001, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i001));

  RLIMenuItemInt* i002 = new RLIMenuItemInt(RLI_STR_MENU_002, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i002));

  RLIMenuItemInt* i003 = new RLIMenuItemInt(RLI_STR_MENU_003, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i003));

  RLIMenuItemInt* i004 = new RLIMenuItemInt(RLI_STR_MENU_004, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i004));

  RLIMenuItemInt* i005 = new RLIMenuItemInt(RLI_STR_MENU_005, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i005));

  RLIMenuItemInt* i006 = new RLIMenuItemInt(RLI_STR_MENU_006, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i006));

  RLIMenuItemInt* i007 = new RLIMenuItemInt(RLI_STR_MENU_007, 0, 255, 255);
  m00->add_item(static_cast<RLIMenuItem*>(i007));

  RLIMenuItemList* i008 = new RLIMenuItemList(RLI_STR_MENU_008, 0);
  i008->addVariant(RLI_STR_ARRAY_DAY_DAY);
  i008->addVariant(RLI_STR_ARRAY_DAY_NIGHT);
  m00->add_item(static_cast<RLIMenuItem*>(i008));


  // --------------------------
  RLIMenuItemMenu* m01 = new RLIMenuItemMenu(RLI_STR_MENU_01, m0);
  m0->add_item(m01);

  RLIMenuItemFloat* i010 = new RLIMenuItemFloat(RLI_STR_MENU_010, 0.01f, 8.f, 2.f);
  m01->add_item(static_cast<RLIMenuItem*>(i010));

  RLIMenuItemInt* i011 = new RLIMenuItemInt(RLI_STR_MENU_011, 5, 60, 30);
  m01->add_item(static_cast<RLIMenuItem*>(i011));

  RLIMenuItemInt* i012 = new RLIMenuItemInt(RLI_STR_MENU_012, 5, 60, 30);
  m01->add_item(static_cast<RLIMenuItem*>(i012));

  RLIMenuItemList* i013 = new RLIMenuItemList(RLI_STR_MENU_013, 0);
  i013->addVariant(RLI_STR_ARRAY_TRACK_1);
  i013->addVariant(RLI_STR_ARRAY_TRACK_2);
  i013->addVariant(RLI_STR_ARRAY_TRACK_3);
  i013->addVariant(RLI_STR_ARRAY_TRACK_6);
  i013->addVariant(RLI_STR_ARRAY_TRACK_12);
  m01->add_item(static_cast<RLIMenuItem*>(i013));
  connect(i013, SIGNAL(valueChanged(RLIString)), this, SIGNAL(tailsModeChanged(RLIString)), Qt::QueuedConnection);

  RLIMenuItemList* i014 = new RLIMenuItemList(RLI_STR_MENU_014, 1);
  i014->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i014->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m01->add_item(static_cast<RLIMenuItem*>(i014));

  RLIMenuItemAction* i015 = new RLIMenuItemAction(RLI_STR_MENU_015);
  i015->setLocked(true);
  m01->add_item(static_cast<RLIMenuItem*>(i015));

  RLIMenuItemAction* i016 = new RLIMenuItemAction(RLI_STR_MENU_016);
  i016->setLocked(true);
  m01->add_item(static_cast<RLIMenuItem*>(i016));

  RLIMenuItemList* i017 = new RLIMenuItemList(RLI_STR_MENU_017, 0);
  i017->addVariant(RLI_STR_ARRAY_TVEC_AP_WATER);
  i017->addVariant(RLI_STR_ARRAY_TVEC_AP_GRND);
  i017->setLocked(true);
  m01->add_item(static_cast<RLIMenuItem*>(i017));


  // --------------------------
  RLIMenuItemMenu* m02 = new RLIMenuItemMenu(RLI_STR_MENU_02, m0);
  m0->add_item(m02);

  RLIMenuItemInt* i020 = new RLIMenuItemInt(RLI_STR_MENU_020, 0, 255, 5);
  m02->add_item(static_cast<RLIMenuItem*>(i020));

  RLIMenuItemList* i021 = new RLIMenuItemList(RLI_STR_MENU_021, 0);
  i021->addVariant(RLI_STR_ARRAY_VD_KM);
  i021->addVariant(RLI_STR_ARRAY_VD_NM);
  m02->add_item(static_cast<RLIMenuItem*>(i021));

  RLIMenuItemList* i022 = new RLIMenuItemList(RLI_STR_MENU_022, 0);
  i022->addVariant(RLI_STR_ARRAY_SPEED_MAN);
  i022->addVariant(RLI_STR_ARRAY_SPEED_LOG);
  m02->add_item(static_cast<RLIMenuItem*>(i022));

  RLIMenuItemFloat* i023 = new RLIMenuItemFloat(RLI_STR_MENU_023, 0.f, 90.f, 5.f);
  m02->add_item(i023);

  RLIMenuItemList* i024 = new RLIMenuItemList(RLI_STR_MENU_024, 2);
  i024->addVariant(RLI_STR_ARRAY_DEV_STAB_ATER);
  i024->addVariant(RLI_STR_ARRAY_DEV_STAB_GPS);
  i024->addVariant(RLI_STR_ARRAY_DEV_STAB_DLG);
  i024->addVariant(RLI_STR_ARRAY_DEV_STAB_L_G_W);
  m02->add_item(i024);

  RLIMenuItemInt* i025 = new RLIMenuItemInt(RLI_STR_MENU_025, 0, 90, 0);
  m02->add_item(i025);

  RLIMenuItemList* i026 = new RLIMenuItemList(RLI_STR_MENU_026, 1);
  i026->addVariant(RLI_STR_ARRAY_LANG_ENGL);
  i026->addVariant(RLI_STR_ARRAY_LANG_RUS);
  connect(i026, SIGNAL(valueChanged(RLIString)), this, SIGNAL(languageChanged(RLIString)), Qt::QueuedConnection);
  m02->add_item(i026);

  RLIMenuItemFloat* i027 = new RLIMenuItemFloat(RLI_STR_MENU_027, 0.f, 359.9f, 0.f);
  m02->add_item(i027);

  RLIMenuItemInt* i028 = new RLIMenuItemInt(RLI_STR_MENU_028, 1, 100, 1);
  m02->add_item(i028);

  RLIMenuItemInt* i029 = new RLIMenuItemInt(RLI_STR_MENU_029, 1, 100, 1);
  i029->setLocked(true);
  m02->add_item(i029);

  RLIMenuItemList* i02A = new RLIMenuItemList(RLI_STR_MENU_02A, 0);
  i02A->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i02A->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m02->add_item(i02A);


  // --------------------------
  RLIMenuItemMenu* m03 = new RLIMenuItemMenu(RLI_STR_MENU_03, m0);
  m0->add_item(m03);

  RLIMenuItemInt* i030 = new RLIMenuItemInt(RLI_STR_MENU_030, 1, 2, 1);
  m03->add_item(static_cast<RLIMenuItem*>(i030));

  RLIMenuItemList* i031 = new RLIMenuItemList(RLI_STR_MENU_031, 0);
  i031->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i031->addVariant(RLI_STR_ARRAY_OFFON_ON);
  connect(i031, SIGNAL(valueChanged(RLIString)), this, SIGNAL(simulationChanged(RLIString)), Qt::QueuedConnection);
  m03->add_item(static_cast<RLIMenuItem*>(i031));

  RLIMenuItemList* i032 = new RLIMenuItemList(RLI_STR_MENU_032, 1);
  i032->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i032->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m03->add_item(static_cast<RLIMenuItem*>(i032));

  RLIMenuItemList* i033 = new RLIMenuItemList(RLI_STR_MENU_033, 1);
  i033->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i033->addVariant(RLI_STR_ARRAY_OFFON_ON);
  m03->add_item(static_cast<RLIMenuItem*>(i033));

  RLIMenuItemFloat* i034 = new RLIMenuItemFloat(RLI_STR_MENU_034, 0.f, 3.f, 0.f);
  m03->add_item(static_cast<RLIMenuItem*>(i034));

  RLIMenuItemList* i035 = new RLIMenuItemList(RLI_STR_MENU_035, 0);
  i035->addVariant(RLI_STR_ARRAY_YESNO_YES);
  i035->addVariant(RLI_STR_ARRAY_YESNO_NO);
  m03->add_item(static_cast<RLIMenuItem*>(i035));

  RLIMenuItemList* i036 = new RLIMenuItemList(RLI_STR_MENU_036, 1);
  i036->addVariant(RLI_STR_ARRAY_YESNO_YES);
  i036->addVariant(RLI_STR_ARRAY_YESNO_NO);
  i036->setEnabled(false);
  m03->add_item(static_cast<RLIMenuItem*>(i036));

  RLIMenuItemList* i037 = new RLIMenuItemList(RLI_STR_MENU_037, 1);
  i037->addVariant(RLI_STR_ARRAY_YESNO_YES);
  i037->addVariant(RLI_STR_ARRAY_YESNO_NO);
  i037->setEnabled(false);
  m03->add_item(static_cast<RLIMenuItem*>(i037));

  RLIMenuItemList* i038 = new RLIMenuItemList(RLI_STR_MENU_038, 1);
  i038->addVariant(RLI_STR_ARRAY_YESNO_YES);
  i038->addVariant(RLI_STR_ARRAY_YESNO_NO);
  i038->setEnabled(false);
  m03->add_item(static_cast<RLIMenuItem*>(i038));


  // --------------------------
  RLIMenuItemMenu* m04 = new RLIMenuItemMenu(RLI_STR_MENU_04, m0);
  m0->add_item(m04);

  RLIMenuItemInt* i040 = new RLIMenuItemInt(RLI_STR_MENU_040, 1, 4, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i040));
  routeLoaderItem = i040;

  RLIMenuItemInt* i041 = new RLIMenuItemInt(RLI_STR_MENU_041, 0, 10, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i041));

  RLIMenuItemAction* i042 = new RLIMenuItemAction(RLI_STR_MENU_042);
  i042->setLocked(true);
  m04->add_item(static_cast<RLIMenuItem*>(i042));

  RLIMenuItemInt* i043 = new RLIMenuItemInt(RLI_STR_MENU_043, 40, 1000, 200);
  m04->add_item(static_cast<RLIMenuItem*>(i043));

  RLIMenuItemAction* i044 = new RLIMenuItemAction(RLI_STR_MENU_044);
  m04->add_item(static_cast<RLIMenuItem*>(i044));
  routeEditItem = i044;

  RLIMenuItemInt* i045 = new RLIMenuItemInt(RLI_STR_MENU_045, 0, 10, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i045));

  RLIMenuItemList* i046 = new RLIMenuItemList(RLI_STR_MENU_046, 0);
  i046->addVariant(RLI_STR_ARRAY_NAME_SYMB_BUOY);
  i046->addVariant(RLI_STR_ARRAY_NAME_SYMB_MILESTONE);
  i046->addVariant(RLI_STR_ARRAY_NAME_SYMB_UNDERWATER_DAMAGE);
  i046->addVariant(RLI_STR_ARRAY_NAME_SYMB_ANCHORAGE);
  i046->addVariant(RLI_STR_ARRAY_NAME_SYMB_COASTAL_LANDMARK);
  m04->add_item(static_cast<RLIMenuItem*>(i046));

  RLIMenuItemInt* i047 = new RLIMenuItemInt(RLI_STR_MENU_047, 1, 4, 1);
  m04->add_item(static_cast<RLIMenuItem*>(i047));
  routeSaverItem = i047;


  // --------------------------
  RLIMenuItemMenu* m05 = new RLIMenuItemMenu(RLI_STR_MENU_05, m0);
  m0->add_item(m05);

  RLIMenuItemList* i050 = new RLIMenuItemList(RLI_STR_MENU_050, 0);
  i050->addVariant(RLI_STR_ARRAY_NAME_SIGN_UNINDENT);
  i050->addVariant(RLI_STR_ARRAY_NAME_SIGN_FRIENDLY);
  i050->addVariant(RLI_STR_ARRAY_NAME_SIGN_ENEMY);
  m05->add_item(static_cast<RLIMenuItem*>(i050));

  RLIMenuItemList* i051 = new RLIMenuItemList(RLI_STR_MENU_051, 0);
  i051->addVariant(RLI_STR_ARRAY_NAME_RECOG_OFF);
  i051->addVariant(RLI_STR_ARRAY_NAME_RECOG_ROUND);
  i051->addVariant(RLI_STR_ARRAY_NAME_RECOG_SECT);
  m05->add_item(static_cast<RLIMenuItem*>(i051));

  RLIMenuItemList* i052 = new RLIMenuItemList(RLI_STR_MENU_052, 0);
  i052->addVariant(RLI_STR_ARRAY_OFFON_OFF);
  i052->addVariant(RLI_STR_ARRAY_OFFON_ON);
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

void MenuEngine::onStateChanged(RLIWidgetState state) {
  switch (state) {
  case RLIWidgetState::RLISTATE_MAIN_MENU:
      _menu = _main_menu;
      break;
  case RLIWidgetState::RLISTATE_CONFIG_MENU:
      _menu = _cnfg_menu;
      break;
  case RLIWidgetState::RLISTATE_DEFAULT:
  case RLIWidgetState::RLISTATE_MAGNIFIER:
      _selection_active = false;
      _menu = nullptr;
      break;
  case RLIWidgetState::RLISTATE_ROUTE_EDITION:
  default:
      break;
  }

  _selected_line = 1;
  _need_update = true;
  _last_action_time = QDateTime::currentDateTime();
}

void MenuEngine::onAnalogZeroChanged(int val) {
  analogZeroItem->setValue(val);
}


void MenuEngine::onLanguageChanged(RLIString lang_str) {
  if (lang_str == RLI_STR_ARRAY_LANG_ENGL)
    _lang = RLI_LANG_ENGLISH;

  if (lang_str == RLI_STR_ARRAY_LANG_RUS)
    _lang = RLI_LANG_RUSSIAN;

  _need_update = true;
}

void MenuEngine::onKeyPressed(QKeyEvent* event) {
  switch(event->key()) {
  // Выбор цели
  case Qt::Key_Up:
    onUp();
    break;

  // ЛИД / ЛОД
  case Qt::Key_Down:
    onDown();
    break;

  // Захват
  case Qt::Key_Return:
  case Qt::Key_Enter:
    onEnter();
    break;

  //Сброс
  case Qt::Key_Escape:
    onBack();
    break;

  }
}

void MenuEngine::onUp() {
  if (_menu == nullptr)
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
  if (_menu == nullptr)
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
        emit loadRoute(routeLoaderItem->intValue()-1);

      if (_menu->item(_selected_line - 1) == routeSaverItem)
        emit saveRoute(routeSaverItem->intValue()-1);

    }
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onBack() {
  if (!_selection_active && _menu->parent() != nullptr) {
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


void MenuEngine::resize(const RLIMenuLayout& layout) {
  _geometry = layout.geometry;
  _font_tag = layout.font;

  if (_fbo != nullptr)
    delete _fbo;

  _fbo = new QOpenGLFramebufferObject(_geometry.size());

  _need_update = true;
}

void MenuEngine::update() {
  if (!_need_update)
    return;

  glViewport(0, 0, _geometry.width(), _geometry.height());

  glEnable(GL_BLEND);

  _fbo->bind();

  glClearColor(MENU_BACKGRD_COLOR.redF(), MENU_BACKGRD_COLOR.greenF(), MENU_BACKGRD_COLOR.blueF(), 1.f);
  glClear(GL_COLOR_BUFFER_BIT);


  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, _geometry.width(), 0.f, _geometry.height(), -1.f, 1.f);

  _prog->bind();

  _prog->setUniformValue(_uniform_locs[INFO_UNIF_MVP], projection);

  // Border
  drawRect(QRect(QPoint(0, 0), QSize(_geometry.width(), 1)), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(0, 0), QSize(1, _geometry.height())), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(0, _geometry.height()-1), QSize(_geometry.width(), 1)), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(_geometry.width()-1, 0), QSize(1, _geometry.height())), MENU_BORDER_COLOR);

  if (_menu != nullptr) {
    QSize font_size = _fonts->getFontSize(_font_tag);

    // Header separator
    drawRect(QRect(QPoint(0, font_size.height() + 6), QSize(_geometry.width(), 1)), MENU_BORDER_COLOR);
    // Footer separator
    drawRect(QRect(QPoint(0, _geometry.height() - font_size.height() - 6), QSize(_geometry.width(), 1)), MENU_BORDER_COLOR);

    // Header
    drawText(_menu->name(_lang), 0, ALIGN_CENTER, MENU_TEXT_STATIC_COLOR);

    // Menu
    for (int i = 0; i < _menu->item_count(); i++) {
//      if ((_menu->item(i) == routeLoaderItem || _menu->item(i) == routeSaverItem) && _routeEngine != nullptr) {
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
  QSize  size = QSize(_geometry.width() - 4, font_size.height() + 4);

  drawRect(QRect(anchor, QSize(size.width(), 1)), col);
  drawRect(QRect(anchor, QSize(1, size.height())), col);
  drawRect(QRect(anchor + QPoint(0, size.height() - 1), QSize(size.width(), 1)), col);
  drawRect(QRect(anchor + QPoint(size.width()-1, 0), QSize(1, size.height())), col);
}

void MenuEngine::drawBar() {
  if (_menu == nullptr)
    return;

  QSize size = _fbo->size();
  int bar_width = 0;

  RLIMenuItem* currItem = _menu->item(_selected_line - 1);

  RLIMenuItemInt* intItem = dynamic_cast<RLIMenuItemInt*>(currItem);
  if (intItem != nullptr) {
    int val = intItem->intValue();
    int min_val = intItem->minValue();
    int max_val = intItem->maxValue();
    bar_width = (size.width() - 2) * static_cast<float>(val - min_val) / (max_val - min_val);
  }

  RLIMenuItemFloat* fltItem = dynamic_cast<RLIMenuItemFloat*>(currItem);
  if (fltItem != nullptr) {
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
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
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
      anchor.setX(_geometry.width()/2 - text.size()*font_size.width()/2);
      break;
    case ALIGN_LEFT:
      anchor.setX(4);
      break;
    case ALIGN_RIGHT:
      anchor.setX(_geometry.width() - 4 - text.size()*font_size.width());
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
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, ord.size()*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, chars.size()*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, ord.size());

  glBindTexture(GL_TEXTURE_2D, 0);
}


void MenuEngine::initShader() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "info.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "info.frag.glsl");
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
