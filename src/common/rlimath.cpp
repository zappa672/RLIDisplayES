#include "rlimath.h"

#include "QDebug"


std::pair<float, float> RLIMath::pos_to_coords( const std::pair<float, float> center_coords
                                              , const QPoint& center_position
                                              , const QPoint& position
                                              , float scale) {
  QPoint metric_pos = (position - center_position) * scale;

  float lat_rads = radians(center_coords.first);

  float lat = degrees(-metric_pos.y() / EARTH_RADIUS) + center_coords.first;
  float lon = degrees(metric_pos.x() / (EARTH_RADIUS * cos(lat_rads))) + center_coords.second;

  return { lat, lon };
}

QPoint RLIMath::coords_to_pos( const std::pair<float, float> center_coords
                             , const std::pair<float, float> coords
                             , QPoint center_position
                             , float scale) {
  float lat_rads = radians(center_coords.first);

  float y_m = -EARTH_RADIUS*radians(coords.first - center_coords.first);
  float x_m = EARTH_RADIUS*cos(lat_rads)*radians(coords.second - center_coords.second);

  QPoint pix_pos = QPoint(floor(x_m / scale), floor(y_m / scale));

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
