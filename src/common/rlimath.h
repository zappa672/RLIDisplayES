#ifndef RLIMATH_H
#define RLIMATH_H

#include <QVector2D>
#include <QPoint>

struct GeoPos {
  double lat, lon;

  inline QVector2D toVec2D() {
    return QVector2D { static_cast<float>(lat), static_cast<float>(lon) };
  }

  GeoPos(double latitude, double longtitude):
    lat{latitude},
    lon{longtitude}
  {}

  GeoPos(const QVector2D& v):
    lat{static_cast<double>(v.x())},
    lon{static_cast<double>(v.y())}
  {}

  GeoPos(QVector2D&& v):
    lat{static_cast<double>(v.x())},
    lon{static_cast<double>(v.y())}
  {}
};


namespace RLIMath {
  constexpr double EPS        = 0.000000000005;

  constexpr double PI         = 3.14159265359;
  constexpr double TWOPI      = 2 * PI;
  constexpr double DEG2RAD    = PI / 180;
  constexpr double RAD2DEG    = 180 / PI;
  constexpr double ERADKM     = 6378.135;
  constexpr double ERADM      = 6378135.0;
  constexpr double AVG_ERAD   = 6371.0;
  constexpr double FLATTENING = 1.0/298.26; // Earth flattening (WGS '72)
  constexpr double MILE2METER = 1852.0;
  constexpr double KM2MILE    = 1 / MILE2METER;


  constexpr inline double rads(double deg) { return deg * DEG2RAD; }
  constexpr inline double degs(double rad) { return rad * RAD2DEG; }


  GeoPos pos_to_coords( const GeoPos& center_coords
                      , const QPointF& center_position
                      , const QPointF& position
                      , double scale );
  QPointF  coords_to_pos( const GeoPos& center_coords
                        , const GeoPos& coords
                        , const QPointF& center_position
                        , double scale );

  // Great circle distance and azimuth
  double GCDistance         (const GeoPos& p1, const GeoPos& p2);
  double GCDistance         (double lat1, double lon1, double lat2, double lon2);
  double GCAzimuth          (double lat1, double lon1, double lat2, double lon2);

  double ApproxDistance     (double lat1, double lon1, double lat2, double lon2);
  double EllipsoidDistance  (double lat1, double lon1, double lat2, double lon2);
}
#endif // RLIMATH_H
