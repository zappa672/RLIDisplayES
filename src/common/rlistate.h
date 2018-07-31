#ifndef RLISTATE_H
#define RLISTATE_H

#include <QPoint>

struct RLIState {
  float gain      = 0.f;
  float water     = 0.f;
  float rain      = 0.f;
  float apch      = 0.f;
  float emission  = 0.f;

  float north_shift = 0.f;
  float chart_scale = 100.f;
  std::pair<float,float> ship_position { 0.0, 0.0 };
  QPoint center_shift { 0, 0 };

  void save();
  void restore();
};

#endif // RLISTATE_H
