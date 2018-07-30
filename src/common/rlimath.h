#ifndef RLIMATH_H
#define RLIMATH_H

#include <qmath.h>
#include <QVector2D>
#include <QPoint>


namespace RLIMath {
  constexpr double PI = 2 * asin(1);
  constexpr double EARTH_RADIUS = 6378137.0;
  constexpr double METERS_PER_MILE = 1852.f;

  inline double radians(double deg) { return (deg / 180.f) * PI; }
  inline double degrees(double rad) { return (rad / PI) * 180.f; }

  std::pair<float, float> pos_to_coords( const std::pair<float, float> center_coords
                                       , const QPoint& center_position
                                       , const QPoint& position
                                       , float scale);
  QPoint coords_to_pos( const std::pair<float, float> center_coords
                      , const std::pair<float, float> coords
                      , QPoint center_position
                      , float scale);

  double geo_distance(double lat1, double lon1, double lat2, double lon2);
  double geo_distance(QVector2D coords1, QVector2D coords2);

}
#endif // RLIMATH_H
