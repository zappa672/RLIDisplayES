#ifndef RLISTATE_H
#define RLISTATE_H

#include <QObject>
#include <QVector2D>
#include <QPoint>

#include "../common/radarscale.h"
#include "../common/rlimath.h"

#include "../datasources/shipdatasource.h"

enum class RLIWidgetState {
  RLISTATE_DEFAULT        = 0
, RLISTATE_MAIN_MENU      = 1
, RLISTATE_CONFIG_MENU    = 2
, RLISTATE_MAGNIFIER      = 3
, RLISTATE_ROUTE_EDITION  = 4
};

enum class RLIOrientation {
  RLIORIENT_HEAD     = 0
, RLIORIENT_NORTH    = 1
, RLIORIENT_COURSE   = 2
};

enum class RLIMode : char {
  RLIMODE_T   = 'T'
, RLIMODE_S   = 'S'
, RLIMODE_X   = 'X'
, RLIMODE_M   = 'M'
} ;


struct RLIState {
  RLIWidgetState state        { RLIWidgetState::RLISTATE_DEFAULT };
  RLIOrientation orientation  { RLIOrientation::RLIORIENT_NORTH };
  RLIMode        mode         { RLIMode::RLIMODE_X };

  float gain                { 0.f };
  float water               { 0.f };
  float rain                { 0.f };
  float apch                { 0.f };
  float emission            { 0.f };

  // Chart parameters
  double      north_shift   { 0.0 };
  RadarScale  radar_scale   { };
  double      chart_scale   { 100.0 };
  QPoint      center_shift  { 0, 0 };

  // Ship parameters
  GeoPos  ship_position     { 0, 0 };
  double  ship_speed        { 0 };
  double  ship_course       { 0 };

  // Controls parameters
  GeoPos  visir_center_pos  { 0, 0 };
  double  vn_p              { 0 };
  double  vn_cu             { 0 };
  double  vd                { 0 };
  QPoint  cursor_pos        { 0, 0 };
  bool    show_circles      { false };
  bool    show_parallel     { false };

private:
  void save();
  void restore();
};

#endif // RLISTATE_H
