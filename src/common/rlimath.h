#ifndef RLIMATH_H
#define RLIMATH_H

#include <qmath.h>
#include <QVector2D>
#include <QPoint>


namespace RLIMath {
  const double PI = 2 * asin(1);
  constexpr double EARTH_RADIUS = 6378137.0;
  constexpr double METERS_PER_MILE = 1852.f;

  inline double radians(double deg) { return (deg / 180.f) * PI; }
  inline double degrees(double rad) { return (rad / PI) * 180.f; }

  QVector2D pos_to_coords( const QVector2D& center_coords
                         , const QPointF& center_position
                         , const QPointF& position
                         , float scale);
  QPointF coords_to_pos( const QVector2D& center_coords
                       , const QVector2D& coords
                       , const QPointF& center_position
                       , float scale);

  double geo_distance(double lat1, double lon1, double lat2, double lon2);
  double geo_distance(QVector2D coords1, QVector2D coords2);

}
#endif // RLIMATH_H
