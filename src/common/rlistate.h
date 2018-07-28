#ifndef RLISTATE_H
#define RLISTATE_H

#include <QPoint>

class RLIState
{
public:
  RLIState();
  ~RLIState();

  inline const QPoint& centerShift() const { return _center_shift; }
  inline void setCenterShift(const QPoint& shift) { _center_shift = shift; }

  inline const std::pair<float, float>& shipPosition() const { return _ship_position; }
  inline void setShipPosition(const std::pair<float, float>& pos) { _ship_position = pos; }

  inline float chartScale() const { return _chart_scale; }
  inline void setChartScale(float scale) { _chart_scale = scale; }

  inline float northShift() const { return _north_shift; }
  inline void setNorthShift(float shift) { _north_shift = shift; }

private:
  float _north_shift;
  float _chart_scale;
  std::pair<float,float> _ship_position;
  QPoint _center_shift;
};

#endif // RLISTATE_H
