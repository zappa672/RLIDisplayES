#include "rlimath.h"

#include "QDebug"

QVector2D RLIMath::pos_to_coords( const QVector2D& center_coords
                                , const QPointF& center_position
                                , const QPointF& position
                                , float scale) {
  QPointF metric_pos = (position - center_position) * scale;

  float lat_rads = radians(center_coords.x());

  float lat = degrees(-metric_pos.y() / EARTH_RADIUS) + center_coords.x();
  float lon = degrees(metric_pos.x() / (EARTH_RADIUS * cos(lat_rads))) + center_coords.y();

  return { lat, lon };
}

QPointF RLIMath::coords_to_pos( const QVector2D& center_coords
                              , const QVector2D& coords
                              , const QPointF& center_position
                              , float scale) {
  float lat_rads = radians(center_coords.x());

  float y_m = -EARTH_RADIUS*radians(coords.x() - center_coords.x());
  float x_m = EARTH_RADIUS*cos(lat_rads)*radians(coords.y() - center_coords.y());

  QPointF pix_pos(floor(x_m / scale), floor(y_m / scale));

  return pix_pos + center_position;
}


double RLIMath::geo_distance(double lat1, double lon1, double lat2, double lon2) {
  double lat2_rads = radians(lat2);
  double y = -EARTH_RADIUS*radians(lat1 - lat2);
  double x = EARTH_RADIUS*cos(lat2_rads)*radians(lon1 - lon2);
  return sqrt(x*x+y*y);
}

double RLIMath::geo_distance(QVector2D coords1, QVector2D coords2) {
  return geo_distance(coords1.x(), coords1.y(), coords2.x(), coords2.y());
}
