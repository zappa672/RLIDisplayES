#ifndef RLISTATE_H
#define RLISTATE_H

#include <QObject>
#include <QVector2D>
#include <QPoint>
#include <QKeyEvent>

#include "../datasources/shipdatasource.h"

enum class RLIWidgetState {
  RLISTATE_DEFAULT        = 0
, RLISTATE_MAIN_MENU      = 1
, RLISTATE_CONFIG_MENU    = 2
, RLISTATE_MAGNIFIER      = 3
, RLSITATE_ROUTE_EDITION  = 4
};

struct RLIState {
  RLIWidgetState state { RLIWidgetState::RLISTATE_DEFAULT };

  float gain      { 0.f };
  float water     { 0.f };
  float rain      { 0.f };
  float apch      { 0.f };
  float emission  { 0.f };

  // Chart parameters
  float   north_shift   { 0.f };
  float   chart_scale   { 100.f };
  QPoint  center_shift  { 0, 0 };

  // Ship parameters
  QVector2D ship_position { 0.0, 0.0 };
  float     ship_speed    { 0.0 };
  float     ship_course   { 0.0 };

  // Controls parameters
  float   vn_p          { 0.f };
  float   vn_cu         { 0.f };
  float   vd            { 0.f };
  QPoint  cursor_pos    { 0, 0 };
  bool    show_circles  { false };
  bool    show_parallel { false };

private:
  void save();
  void restore();
};

#endif // RLISTATE_H
