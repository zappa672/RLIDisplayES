#include "rlistrings.h"

#include <QTextCodec>
#include <QTextEncoder>
#include <QTextDecoder>

void RLIStrings::addString(RLIString name, QVector<QString> vals) {
  QVector<QByteArray> enc_vals(RLI_LANG_COUNT);
  for (int i = 0; i < RLI_LANG_COUNT; i++)
    enc_vals.insert(i, _encoder->fromUnicode(vals[i]));
  _strings.insert(name, enc_vals);
}

RLIStrings::RLIStrings() {
  _encoder = QTextCodec::codecForName("cp866")->makeEncoder();

  addString( RLI_STR_BLANK, {"", ""} );

  // для главного меню
  addString( RLI_STR_MENU_0, { "MAIN MENU", "ГЛАВНОЕ МЕНЮ" } );

  addString( RLI_STR_MENU_00, { "BRIGHTNESS", "ЯРКОСТЬ" } );
  addString( RLI_STR_MENU_01, { "AUTO PLOTTING", "АВТОПРОКЛАДКА" } );
  addString( RLI_STR_MENU_02, { "SETTINGS", "УСТАНОВКИ" } );
  addString( RLI_STR_MENU_03, { "RADAR TEST", "КОНТРОЛЬ" } );
  addString( RLI_STR_MENU_04, { "MAP", "КАРТА" } );
  addString( RLI_STR_MENU_05, { "RECOGNITION", "ОПОЗНАВАНИЕ" } );

  addString( RLI_STR_MENU_000, { "RADAR PICTURE", "РЛ ВИДЕО " } );
  addString( RLI_STR_MENU_001, { "MEASURES", "ИЗМЕРИТ СРЕДСТВА" } );
  addString( RLI_STR_MENU_002, { "FCC", "МД" } );
  addString( RLI_STR_MENU_003, { "PLOT SYMBOLS", "СИМВОЛЫ АРП" } );
  addString( RLI_STR_MENU_004, { "TAILS", "СЛЕДЫ" } );
  addString( RLI_STR_MENU_005, { "SCREEN PANEL", "ПАНЕЛЬ" } );
  addString( RLI_STR_MENU_006, { "CONTROL PANEL", "ПУЛЬТ" } );
  addString( RLI_STR_MENU_007, { "MAP", "КАРТА" } );
  addString( RLI_STR_MENU_008, { "DAY/NIGHT", "ДЕНЬ/НОЧЬ" } );

  addString( RLI_STR_MENU_010, { "CPA  LIMIT nm", "ОПАСН ДКС миль" } );
  addString( RLI_STR_MENU_011, { "TCPA LIMITmin", "ОПАСН ВКС  мин" } );
  addString( RLI_STR_MENU_012, { "VECTORS    min", "ВЕКТОРЫ    мин" } );
  addString( RLI_STR_MENU_013, { "TAILS/PNT min", "СЛЕДЫ/ТЧК  мин" } );
  addString( RLI_STR_MENU_014, { "NUMBER TARGET", "НОМЕРА ЦЕЛЕЙ" } );
  addString( RLI_STR_MENU_015, { "AZ DELETE", "СБРОС ЗОНЫ" } );
  addString( RLI_STR_MENU_016, { "AZ SETTING", "УСТАН ЗОНЫ" } );
  addString( RLI_STR_MENU_017, { "TRUE VECTOR", "ЛИД" } );

  addString( RLI_STR_MENU_020, { "TUNE", "РПЧ" } );
  addString( RLI_STR_MENU_021, { "CHANGE VRM", "СМЕНА ВД" } );
  addString( RLI_STR_MENU_022, { "SPEED DEVICE", "ДАТЧИК СКОР" } );
  addString( RLI_STR_MENU_023, { "MAN SPD kts ", "СК  РУЧ  уз " } );
  addString( RLI_STR_MENU_024, { "STAB SYSTEM", "ДАТЧИК СТАБ" } );
  addString( RLI_STR_MENU_025, { "TIMER min", "ТАЙМЕР мин" } );
  addString( RLI_STR_MENU_026, { "LANGUAGE", "ЯЗЫК" } );
  addString( RLI_STR_MENU_027, { "GYRO ALIGN", "СОГЛАС ГИРО" } );
  addString( RLI_STR_MENU_028, { "DANGER DPT m","ОПАС ГЛУБ м" } );
  addString( RLI_STR_MENU_029, { "ANCHORAGE m", "ЯКОРНАЯ СТ м" } );
  addString( RLI_STR_MENU_02A, { "STAB NOISE", "ШУМ СТАБ" } );

  addString( RLI_STR_MENU_030, { "STROBE AT", "СТРОБ АС" } );
  addString( RLI_STR_MENU_031, { "TARGETS SIMUL", "ИМИТАЦИЯ ЦЕЛИ" } );
  addString( RLI_STR_MENU_032, { "PERFORM MON", "ЭП" } );
  addString( RLI_STR_MENU_033, { "PEAK POWER", "КИМ" } );
  addString( RLI_STR_MENU_034, { "MAGNETRON", "ТОК МАГНЕТР" } );
  addString( RLI_STR_MENU_035, { "DISP TRIG", "ЗАПУСК И" } );
  addString( RLI_STR_MENU_036, { "RADAR VIDEO", "РЛ ВИДЕО" } );
  addString( RLI_STR_MENU_037, { "AZ PULSE", "КУА" } );
  addString( RLI_STR_MENU_038, { "HEAD PULSE", "ОК" } );

  addString( RLI_STR_MENU_040, { "CALL ROUTE №", "ВЫЗОВ КАРТЫ №" } );
  addString( RLI_STR_MENU_041, { "TAILS/PNT", "СЛЕДЫ/ТЧК" } );
  addString( RLI_STR_MENU_042, { "-------------------", "-------------------" } );
  addString( RLI_STR_MENU_043, { "FARWATER m", "ФАРВАТЕР м" } );
  addString( RLI_STR_MENU_044, { "LINE OF A ROUTE", "ЛИНИЯ МАРШРУТА" } );
  addString( RLI_STR_MENU_045, { "POINT NUMBER", "НОМЕР ТОЧКИ" } );
  addString( RLI_STR_MENU_046, { "SYMBOLS", "СИМВОЛЫ" } );
  addString( RLI_STR_MENU_047, { "RECORDING MAP №", "ЗАПИСЬ КАРТЫ №" } );

  addString( RLI_STR_MENU_050, { "SIGN", "ПРИЗНАК" } );
  addString( RLI_STR_MENU_051, { "REQUEST", "ЗАПРОС" } );
  addString( RLI_STR_MENU_052, { "ABORT EMIS", "ЗАПРЕТ ИЗЛУЧ" } );


  // для меню конфигурация
  addString( RLI_STR_MENU_1, { "CONFIG", "КОНФИГУРАЦИЯ" } );

  addString( RLI_STR_MENU_10, { "TUNINGS", "НАСТРОЙКИ" } );
  addString( RLI_STR_MENU_11, { "TUNINGS PIKO", "НАСТРОЙКИ ПИКО" } );
  addString( RLI_STR_MENU_12, { "NMEA ACCEPT", "ПРИЕМ NMEA" } );
  addString( RLI_STR_MENU_13, { "NMEA TRANSM", "ПЕРЕДАЧА NMEA" } );

  addString( RLI_STR_MENU_100, { "LOG SIGNAL", "СИГНАЛ ЛАГА" } );
  addString( RLI_STR_MENU_101, { "RANGE CCN", "КОР ДАЛЬН" } );
  addString( RLI_STR_MENU_102, { "BEARING CCN", "КОР НАПР" } );
  addString( RLI_STR_MENU_103, { "BEGIN TR OFF", "НАЧ ОТКЛ ПП" } );
  addString( RLI_STR_MENU_104, { "END TR OFF", "КОН ОТКЛ ПП" } );
  addString( RLI_STR_MENU_105, { "BAND    ", "ДИАПАЗОН" } );
  addString( RLI_STR_MENU_106, { "GRAPH AFC", "ГРАФИК АПЧ" } );
  addString( RLI_STR_MENU_107, { "MASTER BRIGHTN", "ОБЩАЯ ЯРКОСТЬ" } );

  addString( RLI_STR_MENU_110, { "MAX VIDEO", "ДИН ДИАП ВС" } );
  addString( RLI_STR_MENU_111, { "TOP MARGIN", "ОГРАН ВЕРХОВ" } );
  addString( RLI_STR_MENU_112, { "ANALOG ZERO", "АНАЛОГ 0 ВС" } );
  addString( RLI_STR_MENU_113, { "NOISE MARGIN", "ОГРАН ШУМОВ" } );
  addString( RLI_STR_MENU_114, { "TRESH AT", "ПОРОГ АС" } );
  addString( RLI_STR_MENU_115, { "TRESH AT", "ПОРОГ СЛЕДОВ" } );
  addString( RLI_STR_MENU_116, { "THRESH ZONE", "ПОРОГ ЗОНЫ" } );
  addString( RLI_STR_MENU_117, { "THRESH ACCUM", "ПОР ЯРК НАКОП" } );
  addString( RLI_STR_MENU_118, { "DELTA ARPA 6", "ДЕЛЬТА САРП 6" } );
  addString( RLI_STR_MENU_119, { "DELTA ZONE 6", "ДЕЛЬТА ЗОНЫ 6" } );
  addString( RLI_STR_MENU_11A, { "DELTA ZONE 12", "ДЕЛЬТА ЗОНЫ 12" } );

  addString( RLI_STR_MENU_120, { "GGA ", "GGA " } );
  addString( RLI_STR_MENU_121, { "VTG ", "VTG " } );
  addString( RLI_STR_MENU_122, { "ZDA ", "ZDA " } );
  addString( RLI_STR_MENU_123, { "DPT ", "DPT " } );
  addString( RLI_STR_MENU_124, { "DBT ", "DBT " } );
  addString( RLI_STR_MENU_125, { "VBW ", "VBW " } );
  addString( RLI_STR_MENU_126, { "VHW ", "VHW " } );
  addString( RLI_STR_MENU_127, { "RMC ", "RMC " } );

  addString( RLI_STR_MENU_130, { "TTM", "TTM" } );
  addString( RLI_STR_MENU_131, { "VHW", "VHW" } );
  addString( RLI_STR_MENU_132, { "RSD", "RSD" } );
  addString( RLI_STR_MENU_133, { "BAUDRATE", "СК ОБМЕНА" } );

  addString( RLI_STR_EMISSION,    { "EMISSION",	"ИЗЛУЧЕНИЕ" } );
  addString( RLI_STR_DEGREE_SIGN, { "°   ",	"°   " } );
  addString( RLI_STR_GAIN,        { "GAIN", "УСИЛЕНИЕ" } );
  addString( RLI_STR_RAIN,        { "RAIN", "ДОЖДЬ" } );
  addString( RLI_STR_RAIN_1,      { "RAIN 1", "ДОЖДЬ 1" } );
  addString( RLI_STR_WAVE,        { " SEA ", "ВОЛНЫ" } );
  addString( RLI_STR_AFC,         { "AFC", "АПЧ" } );
  addString( RLI_STR_TUNE,        { "TUNE", "РПЧ" } );
  addString( RLI_STR_PP12p,       { "_П___", "_П___" } );
  addString( RLI_STR_MODE,        { "MODE", "РЕЖИМЫ" } );
  addString( RLI_STR_HEAD,        { "HEAD", "КУРС" } );
  addString( RLI_STR_NORTH,       { "NORTH", "СЕВЕР" } );
  addString( RLI_STR_COURSE,      { "COURSE", "КУРС СТ" } );
  addString( RLI_STR_RM,          { "RM", "ОД" } );
  addString( RLI_STR_TM,          { "TM", "ИД" } );
  addString( RLI_STR_GROUND,      { "GROUND", "ГРУНТ" } );
  addString( RLI_STR_WATER,       { "WATER", "ВОДА" } );
  addString( RLI_STR_EBL,         { "EBL", "ВН" } );
  addString( RLI_STR_B,           { "B ", "П " } );
  addString( RLI_STR_TAILS,       { "TAILS", "СЛЕДЫ" } );
  addString( RLI_STR_POINTS,      { "POINTS", "ТОЧКИ" } );
  addString( RLI_STR_MIN,         { "MIN ", "МИН " } );
  addString( RLI_STR_VRM,         { "VRM", "ВД" } );
  addString( RLI_STR_BLINK,       { "  ", "  " } );
  addString( RLI_STR_NM,          { "NM  ", "МИЛЬ" } );
  addString( RLI_STR_KM,          { "KM  ", "КМ  " } );
  addString( RLI_STR_METER,       { "M   ", "М   " } );
  addString( RLI_STR_GYRO_HDG,    { "GYRO HDG ", "КУРС ГИРО" } );
  addString( RLI_STR_GYRO_OFF,    { "GYRO OFF ", "КУРС ОТКЛ" } );
  addString( RLI_STR_LOG_SPD_S,   { "LOG SPD(S)", "СК ЛАГ(С) " } );
  addString( RLI_STR_LOG_SPD_W,   { "LOG SPD(W)", "СК ЛАГ(В) " } );
  addString( RLI_STR_MAN_SPD,     { "MAN SPD   ", "СК РУЧ    " } );
  addString( RLI_STR_KTS,         { "KTS ", "УЗ  " } );
  addString( RLI_STR_KM_H,        { "KM/H", "КМ/Ч" } );
  addString( RLI_STR_LAT,         { "LAT    ", "ШИРОТА " } );
  addString( RLI_STR_NULL,        { "", "" } );
  addString( RLI_STR_LON,         { "LON    ", "ДОЛГОТА" } );
  addString( RLI_STR_ACT_COURSE,  { "ACT COURSE  ", "КУРС АОЦ  " } );
  addString( RLI_STR_GPS_COURSE,  { "GPS COURSE  ", "КУРС СНС  " } );
  addString( RLI_STR_DLG_COURSE,  { "DLG COURSE  ", "КУРС ДЛГ  " } );
  addString( RLI_STR_ACT_SPEED,   { "ACT SPEED", "СКОР АОЦ " } );
  addString( RLI_STR_GPS_SPEED,   { "GPS SPEED", "СКОР СНС " } );
  addString( RLI_STR_DLG_SPEED,   { "DLG SPEED", "СКОР ДЛГ " } );
  addString( RLI_STR_VECTOR,      { "VECTOR", "ВЕКТОР" } );
  addString( RLI_STR_DANGER_TRG,  { "DANGER TRG", "ОПАСНАЯ ЦЕЛЬ" } );
  addString( RLI_STR_CPA_LIMIT,   { "CPA LIMIT  ", "ОПАСНАЯ ДКС" } );
  addString( RLI_STR_TCPA_LIMIT,  { "TCPA LIMIT ", "ОПАСНОЕ ВКС" } );
  addString( RLI_STR_TRG_N_ALL,   { "TRG  №    ALL", "ЦЕЛЬ №    ВСЕГО" } );
  addString( RLI_STR_TRG_ALL,     { "TRG       ALL", "ЦЕЛЬ      ВСЕГО" } );
  addString( RLI_STR_BEARING,     { "BEARING  ", "ПЕЛЕНГ   " } );
  addString( RLI_STR_RANGE,       { "RANGE    ", "ДАЛЬНОСТЬ" } );
  addString( RLI_STR_COURSE_W,    { "COURSE(W)", "КУРС  (В)" } );
  addString( RLI_STR_SPEED_W,     { "SPEED (W)", "СКОР  (В)" } );
  addString( RLI_STR_COURSE_G,    { "COURSE(G)", "КУРС  (Г)" } );
  addString( RLI_STR_SPEED_G,     { "SPEED (G)", "СКОР  (Г)" } );
  addString( RLI_STR_CPA,         { "CPA", "ДКС" } );
  addString( RLI_STR_TCPA,        { "TCPA", "ВКС " } );
  addString( RLI_STR_DCC,         { "DCC", "ДПК" } );
  addString( RLI_STR_TCC,         { "TCC", "ВПК" } );
  addString( RLI_STR_CURSOR,      { "CURSOR", "КУРСОР" } );
  addString( RLI_STR_HB,          { "HB       ", "КУРС.УГОЛ" } );
  addString( RLI_STR_OFF,         { "OFF", "ОТКЛ" } );
  addString( RLI_STR_ON,          { "ON", " ВКЛ" } );
  addString( RLI_STR_AUTO_A_C,    { "AUTO A/C", "АПЗ" } );
  addString( RLI_STR_X_BAND,      { "X-BAND", "Х-ДИАП" } );
  addString( RLI_STR_S_BAND,      { "S-BAND", "S-ДИАП" } );
  addString( RLI_STR_K_BAND,      { "K-BAND", "K-ДИАП" } );

  addString( RLI_STR_LOD,         { "???", "ЛОД" } );
  addString( RLI_STR_CU,          { "??", "КУ" } );
  addString( RLI_STR_GRAD_LB,     { "°LB",	"°ЛБ" } );
  addString( RLI_STR_GRAD_RB,     { "°RB",	"°ПБ" } );

  addString( RLI_STR_TIME,        { "TIME", "ВРЕМЯ" } );
  addString( RLI_STR_FPS,         { "FPS", "КАДР/С"});

  // для маршрута
  addString( RLI_STR_GO_TO_RP,    { "GO TO RP", "ИДЕМ НА МТ" } );
  addString( RLI_STR_PARAM_OF_A_ROUTE,   { "PARAM OF A ROUTE", "ПАРАМЕТРЫ МАРШРУТА" } );
  addString( RLI_STR_FAIRWAY_m,   { "FAIRWAY m", "ФАРВАТЕР м" } );
  addString( RLI_STR_DEFLECT,     { "DEFLECT   ", "ОТКЛОНЕН  " } );
  addString( RLI_STR_RANGE_RP,    { "RANGE RP", "ДАЛЬН МТ" } );
  addString( RLI_STR_TIME_RP,     { "TIME  RP", "ВРЕМЯ МТ" } );
  addString( RLI_STR_TA_ON_RP,    { "TA ON RP", "ПУ НА МТ" } );
  addString( RLI_STR_NEW_COURSE,  { "NEW COURSE", "НОВЫЙ КУРС" } );
  addString( RLI_STR_DEPTH,       { "DEPTH", "ГЛУБИНА" } );
  addString( RLI_STR_METRS,       { "METRS", "МЕТРЫ" } );

  // выбор день/ночь
  addString( RLI_STR_ARRAY_DAY_NIGHT, { "NIGHT", "НОЧЬ" } );
  addString( RLI_STR_ARRAY_DAY_DAY,   { "DAY", " ДЕНЬ" } );

  // значения следов/точек для меню
  addString( RLI_STR_ARRAY_TRACK_1,   { "1", "1" } );
  addString( RLI_STR_ARRAY_TRACK_2,   { "2", "2" } );
  addString( RLI_STR_ARRAY_TRACK_3,   { "3", "3" } );
  addString( RLI_STR_ARRAY_TRACK_6,   { "6", "6" } );
  addString( RLI_STR_ARRAY_TRACK_12,  { "12", "12" } );

  // выбор ОТКЛ/ВКЛ
  addString( RLI_STR_ARRAY_OFFON_OFF, { "OFF", "ОТКЛ" } );
  addString( RLI_STR_ARRAY_OFFON_ON,  { "ON", "ВКЛ" } );

  // выбор ДА/НЕТ
  addString( RLI_STR_ARRAY_YESNO_YES, { "YES", "ДА" } );
  addString( RLI_STR_ARRAY_YESNO_NO,  { "NO", "НЕТ" } );

  // выбор ЛИД для АС
  addString( RLI_STR_ARRAY_TVEC_AP_WATER, { "WATER", "ВОДА" } );
  addString( RLI_STR_ARRAY_TVEC_AP_GRND,  { "GRND", "ГРУНТ" } );

  // значения типа скорости для меню
  addString( RLI_STR_ARRAY_SPEED_MAN,   { "MAN", "РУЧ" } );
  addString( RLI_STR_ARRAY_SPEED_LOG,   { "LOG", "ЛАГ" } );

  // выбор языка системы
  addString( RLI_STR_ARRAY_LANG_ENGL,   { "ENGL", "АНГЛ" } );
  addString( RLI_STR_ARRAY_LANG_RUS,    { "RUS", "РУС" } );

  // выбор датчика стабилизации
  addString( RLI_STR_ARRAY_DEV_STAB_ATER,  { "ATER", "АОЦ" } );
  addString( RLI_STR_ARRAY_DEV_STAB_GPS,   { "GPS", "СНС" } );
  addString( RLI_STR_ARRAY_DEV_STAB_DLG,   { "DLG", "ДЛГ" } );
  addString( RLI_STR_ARRAY_DEV_STAB_L_G_W, { "L-G(W)", "ГК-Л(В)" } );

  // выбор единиц измерения ВД
  addString( RLI_STR_ARRAY_VD_KM, { "KM", "КМ" } );
  addString( RLI_STR_ARRAY_VD_NM, { "NM", "МИЛИ" } );

  // символы маршрута
  addString( RLI_STR_ARRAY_NAME_SYMB_BUOY,              { "╚", "╚" } ); // буй                  0xc8 в cp866
  addString( RLI_STR_ARRAY_NAME_SYMB_MILESTONE,         { "╔", "╔" } ); // веха                 0xc9 в cp866
  addString( RLI_STR_ARRAY_NAME_SYMB_UNDERWATER_DAMAGE, { "╩", "╩" } ); // подводная опасность  0xca в cp866
  addString( RLI_STR_ARRAY_NAME_SYMB_ANCHORAGE,         { "╦", "╦" } ); // якорная стоянка      0xcb в cp866
  addString( RLI_STR_ARRAY_NAME_SYMB_COASTAL_LANDMARK,  { "╠", "╠" } ); // береговой ориентир   0xcc в cp866

  // опознавание
  addString( RLI_STR_ARRAY_NAME_RECOG_OFF,    { "OFF", "НЕТ" } );
  addString( RLI_STR_ARRAY_NAME_RECOG_ROUND,  { "ROUND", "КРУГ" } );
  addString( RLI_STR_ARRAY_NAME_RECOG_SECT,   { "SECT", "СЕКТ" } );

  addString( RLI_STR_ARRAY_NAME_SIGN_UNINDENT,  { "UNINDENT", "НЕТ" } );
  addString( RLI_STR_ARRAY_NAME_SIGN_FRIENDLY,  { "FRIENDLY", "СВОЙ" } );
  addString( RLI_STR_ARRAY_NAME_SIGN_ENEMY,     { "ENEMY", "ЧУЖОЙ" } );

  // выбор типа лага
  addString( RLI_STR_ARRAY_LOG_SIGNAL_200_MINUS,  { "200-", "200-" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_500_MINUS,  { "500-", "500-" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_COD_MINUS,  { "COD-", "КОД-" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_NMEA,       { "NMEA", "NMEA" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_COD_PLUS,   { "COD+", "КОД+" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_500_PLUS,   { "500+", "500+" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_200_PLUS,   { "200+", "200+" } );
  addString( RLI_STR_ARRAY_LOG_SIGNAL_GPS,        { "GPS", "СНС" } );

  // выбор диапазона
  addString( RLI_STR_ARRAY_BAND_X, { "X", "X" } );
  addString( RLI_STR_ARRAY_BAND_S, { "S", "S" } );
  addString( RLI_STR_ARRAY_BAND_K, { "K", "K" } );
}

RLIStrings::~RLIStrings(void) {
  for (auto _string_vec : _strings)
    _string_vec.clear();
  _strings.clear();
}
