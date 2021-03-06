#ifndef RLISTRINGNAMES_H
#define RLISTRINGNAMES_H

enum RLIString {
    RLI_STR_NONE
  , RLI_STR_BLANK

  , RLI_STR_MENU_0

  , RLI_STR_MENU_00
  , RLI_STR_MENU_01
  , RLI_STR_MENU_02
  , RLI_STR_MENU_03
  , RLI_STR_MENU_04
  , RLI_STR_MENU_05

  , RLI_STR_MENU_000
  , RLI_STR_MENU_001
  , RLI_STR_MENU_002
  , RLI_STR_MENU_003
  , RLI_STR_MENU_004
  , RLI_STR_MENU_005
  , RLI_STR_MENU_006
  , RLI_STR_MENU_007
  , RLI_STR_MENU_008

  , RLI_STR_MENU_010
  , RLI_STR_MENU_011
  , RLI_STR_MENU_012
  , RLI_STR_MENU_013
  , RLI_STR_MENU_014
  , RLI_STR_MENU_015
  , RLI_STR_MENU_016
  , RLI_STR_MENU_017

  , RLI_STR_MENU_020
  , RLI_STR_MENU_021
  , RLI_STR_MENU_022
  , RLI_STR_MENU_023
  , RLI_STR_MENU_024
  , RLI_STR_MENU_025
  , RLI_STR_MENU_026
  , RLI_STR_MENU_027
  , RLI_STR_MENU_028
  , RLI_STR_MENU_029
  , RLI_STR_MENU_02A

  , RLI_STR_MENU_030
  , RLI_STR_MENU_031
  , RLI_STR_MENU_032
  , RLI_STR_MENU_033
  , RLI_STR_MENU_034
  , RLI_STR_MENU_035
  , RLI_STR_MENU_036
  , RLI_STR_MENU_037
  , RLI_STR_MENU_038

  , RLI_STR_MENU_040
  , RLI_STR_MENU_041
  , RLI_STR_MENU_042
  , RLI_STR_MENU_043
  , RLI_STR_MENU_044
  , RLI_STR_MENU_045
  , RLI_STR_MENU_046
  , RLI_STR_MENU_047

  , RLI_STR_MENU_050
  , RLI_STR_MENU_051
  , RLI_STR_MENU_052

  , RLI_STR_MENU_1

  , RLI_STR_MENU_10
  , RLI_STR_MENU_11
  , RLI_STR_MENU_12
  , RLI_STR_MENU_13

  , RLI_STR_MENU_100
  , RLI_STR_MENU_101
  , RLI_STR_MENU_102
  , RLI_STR_MENU_103
  , RLI_STR_MENU_104
  , RLI_STR_MENU_105
  , RLI_STR_MENU_106
  , RLI_STR_MENU_107

  , RLI_STR_MENU_110
  , RLI_STR_MENU_111
  , RLI_STR_MENU_112
  , RLI_STR_MENU_113
  , RLI_STR_MENU_114
  , RLI_STR_MENU_115
  , RLI_STR_MENU_116
  , RLI_STR_MENU_117
  , RLI_STR_MENU_118
  , RLI_STR_MENU_119
  , RLI_STR_MENU_11A

  , RLI_STR_MENU_120
  , RLI_STR_MENU_121
  , RLI_STR_MENU_122
  , RLI_STR_MENU_123
  , RLI_STR_MENU_124
  , RLI_STR_MENU_125
  , RLI_STR_MENU_126
  , RLI_STR_MENU_127

  , RLI_STR_MENU_130
  , RLI_STR_MENU_131
  , RLI_STR_MENU_132
  , RLI_STR_MENU_133

  , RLI_STR_EMISSION
  , RLI_STR_DEGREE_SIGN
  , RLI_STR_GAIN
  , RLI_STR_RAIN
  , RLI_STR_RAIN_1
  , RLI_STR_WAVE
  , RLI_STR_AFC
  , RLI_STR_TUNE
  , RLI_STR_PP12p
  , RLI_STR_MODE
  , RLI_STR_HEAD
  , RLI_STR_NORTH
  , RLI_STR_COURSE
  , RLI_STR_RM
  , RLI_STR_TM
  , RLI_STR_GROUND
  , RLI_STR_WATER
  , RLI_STR_EBL
  , RLI_STR_B
  , RLI_STR_TAILS
  , RLI_STR_POINTS
  , RLI_STR_MIN
  , RLI_STR_VRM
  , RLI_STR_BLINK
  , RLI_STR_NM
  , RLI_STR_KM
  , RLI_STR_METER
  , RLI_STR_GYRO_HDG
  , RLI_STR_GYRO_OFF
  , RLI_STR_LOG_SPD_S
  , RLI_STR_LOG_SPD_W
  , RLI_STR_MAN_SPD
  , RLI_STR_KTS
  , RLI_STR_KM_H
  , RLI_STR_LAT
  , RLI_STR_NULL
  , RLI_STR_LON
  , RLI_STR_ACT_COURSE
  , RLI_STR_GPS_COURSE
  , RLI_STR_DLG_COURSE
  , RLI_STR_ACT_SPEED
  , RLI_STR_GPS_SPEED
  , RLI_STR_DLG_SPEED
  , RLI_STR_VECTOR
  , RLI_STR_DANGER_TRG
  , RLI_STR_CPA_LIMIT
  , RLI_STR_TCPA_LIMIT
  , RLI_STR_TRG_N_ALL
  , RLI_STR_TRG_ALL
  , RLI_STR_BEARING
  , RLI_STR_RANGE
  , RLI_STR_COURSE_W
  , RLI_STR_SPEED_W
  , RLI_STR_COURSE_G
  , RLI_STR_SPEED_G
  , RLI_STR_CPA
  , RLI_STR_TCPA
  , RLI_STR_DCC
  , RLI_STR_TCC
  , RLI_STR_CURSOR
  , RLI_STR_HB
  , RLI_STR_OFF
  , RLI_STR_ON
  , RLI_STR_AUTO_A_C
  , RLI_STR_X_BAND
  , RLI_STR_S_BAND
  , RLI_STR_K_BAND

  , RLI_STR_LOD
  , RLI_STR_CU
  , RLI_STR_GRAD_LB
  , RLI_STR_GRAD_RB

  , RLI_STR_TIME
  , RLI_STR_FPS

  // для маршрута
  , RLI_STR_GO_TO_RP
  , RLI_STR_PARAM_OF_A_ROUTE
  , RLI_STR_FAIRWAY_m
  , RLI_STR_DEFLECT
  , RLI_STR_RANGE_RP
  , RLI_STR_TIME_RP
  , RLI_STR_TA_ON_RP
  , RLI_STR_NEW_COURSE
  , RLI_STR_DEPTH
  , RLI_STR_METRS

  // выбор день/ночь
  , RLI_STR_ARRAY_DAY_NIGHT
  , RLI_STR_ARRAY_DAY_DAY

  // значения следов/точек для меню
  , RLI_STR_ARRAY_TRACK_1
  , RLI_STR_ARRAY_TRACK_2
  , RLI_STR_ARRAY_TRACK_3
  , RLI_STR_ARRAY_TRACK_6
  , RLI_STR_ARRAY_TRACK_12

  // выбор ОТКЛ/ВКЛ
  , RLI_STR_ARRAY_OFFON_OFF
  , RLI_STR_ARRAY_OFFON_ON

  // выбор ДА/НЕТ
  , RLI_STR_ARRAY_YESNO_YES
  , RLI_STR_ARRAY_YESNO_NO

  // выбор ЛИД для АС
  , RLI_STR_ARRAY_TVEC_AP_WATER
  , RLI_STR_ARRAY_TVEC_AP_GRND

  // значения типа скорости для меню
  , RLI_STR_ARRAY_SPEED_MAN
  , RLI_STR_ARRAY_SPEED_LOG

  // выбор языка системы
  , RLI_STR_ARRAY_LANG_ENGL
  , RLI_STR_ARRAY_LANG_RUS

  // выбор датчика стабилизации
  , RLI_STR_ARRAY_DEV_STAB_ATER
  , RLI_STR_ARRAY_DEV_STAB_GPS
  , RLI_STR_ARRAY_DEV_STAB_DLG
  , RLI_STR_ARRAY_DEV_STAB_L_G_W

  // выбор единиц измерения ВД
  , RLI_STR_ARRAY_VD_KM
  , RLI_STR_ARRAY_VD_NM

  // символы маршрута
  , RLI_STR_ARRAY_NAME_SYMB_BUOY
  , RLI_STR_ARRAY_NAME_SYMB_MILESTONE
  , RLI_STR_ARRAY_NAME_SYMB_UNDERWATER_DAMAGE
  , RLI_STR_ARRAY_NAME_SYMB_ANCHORAGE
  , RLI_STR_ARRAY_NAME_SYMB_COASTAL_LANDMARK

  // опознавание
  , RLI_STR_ARRAY_NAME_RECOG_OFF
  , RLI_STR_ARRAY_NAME_RECOG_ROUND
  , RLI_STR_ARRAY_NAME_RECOG_SECT

  , RLI_STR_ARRAY_NAME_SIGN_UNINDENT
  , RLI_STR_ARRAY_NAME_SIGN_FRIENDLY
  , RLI_STR_ARRAY_NAME_SIGN_ENEMY

  // выбор типа лага
  , RLI_STR_ARRAY_LOG_SIGNAL_200_MINUS
  , RLI_STR_ARRAY_LOG_SIGNAL_500_MINUS
  , RLI_STR_ARRAY_LOG_SIGNAL_COD_MINUS
  , RLI_STR_ARRAY_LOG_SIGNAL_NMEA
  , RLI_STR_ARRAY_LOG_SIGNAL_COD_PLUS
  , RLI_STR_ARRAY_LOG_SIGNAL_500_PLUS
  , RLI_STR_ARRAY_LOG_SIGNAL_200_PLUS
  , RLI_STR_ARRAY_LOG_SIGNAL_GPS

  // выбор диапазона
  , RLI_STR_ARRAY_BAND_X
  , RLI_STR_ARRAY_BAND_S
  , RLI_STR_ARRAY_BAND_K
};



#endif // RLISTRINGNAMES_H
