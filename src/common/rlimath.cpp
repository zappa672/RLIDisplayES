#include "rlimath.h"

#include <cmath>
#include "QDebug"

QPointF RLIMath::intersectRayCircle( QPointF o
                                   , double r
                                   , QPointF p
                                   , double phi) {
  double dy = p.y()-o.y();
  double dx = p.x()-o.x();
  double r2 = r*r;

  QPointF p1, p2;
  double tp, tp2;
  double a, b, c, d;

  // Split solving intersection equation to prevent too big tan
  if (std::abs(sin(phi)) > 0.5) {
    tp = tan(phi - rads(90.0));
    tp2 = tp*tp;

    a = 1.0 + tp2;
    b = 2.0 * (tp*dy - o.x() - tp2*p.x());
    c = dy*dy + o.x()*o.x() + (tp*p.x())*(tp*p.x()) - 2.0*tp*p.x()*dy - r2;

    d = b*b - 4.0*a*c;

    if (d < 0.0) {
      return o;
    } else {
      p1.setX( (-b + sqrt(d)) / (2.0 * a) );
      p2.setX( (-b - sqrt(d)) / (2.0 * a) );

      p1.setY( p.y() + tp * (p1.x() - p.x()) );
      p2.setY( p.y() + tp * (p2.x() - p.x()) );

      // To the right from hole center, choose
      if (sin(phi) * (p1.x() - p.x()) > 0.0)
        return p1;
      else
        return p2;
    }
  } else {
    tp = tan(-phi);
    tp2 = tp*tp;

    a = 1.0 + tp2;
    b = 2.0 * (tp*dx - o.y() - tp2*p.y());
    c = dx*dx + o.y()*o.y() + (tp*p.y())*(tp*p.y()) - 2.0*tp*p.y()*dx - r2;

    d = b*b - 4.0*a*c;

    if (d < 0.0) {
      return o;
    } else {
      p1.setY( (-b + sqrt(d)) / (2.0 * a) );
      p2.setY( (-b - sqrt(d)) / (2.0 * a) );
      p1.setX( p.x() + tp * (p1.y() - p.y()) );
      p2.setX( p.x() + tp * (p2.y() - p.y()) );

      // To the right from hole center, choose
      if (cos(phi) * (p1.y() - p.y()) < 0.0)
        return p1;
      else
        return p2;
    }
  }
}

GeoPos RLIMath::pos_to_coords( const GeoPos& center_coords
                             , const QPointF& center_position
                             , const QPointF& position
                             , double scale ) {
  double erad_pix = ERADM / scale;
  QPointF pos_p = position - center_position;

  double lat = degs(-pos_p.y() / erad_pix) + center_coords.lat;
  double lon = degs(pos_p.x() / (erad_pix * cos(rads(center_coords.lat)))) + center_coords.lon;

  return { lat, lon };
}

QPointF RLIMath::coords_to_pos( const GeoPos& center_coords
                              , const GeoPos& coords
                              , const QPointF& center_position
                              , double scale ) {
  double erad_pix = ERADM / scale;
  double y_p = -erad_pix * rads(coords.lat - center_coords.lat);
  double x_p =  erad_pix * cos(rads(center_coords.lat)) * rads(coords.lon - center_coords.lon);
  return QPointF(x_p, y_p) + center_position;
}



double RLIMath::GCDistance(const GeoPos& p1, const GeoPos& p2) {
  return GCDistance(p1.lat, p1.lon, p2.lat, p2.lon);
}


double RLIMath::GCDistance(double lat1, double lon1, double lat2, double lon2) {
  lat1 = rads(lat1);
  lon1 = rads(lon1);
  lat2 = rads(lat2);
  lon2 = rads(lon2);

  double d = sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(lon1 - lon2);
  return (AVG_ERAD * acos(d));
}


double RLIMath::GCAzimuth(double lat1, double lon1, double lat2, double lon2) {
  double result = 0.0;

  int ilat1 = static_cast<int>(0.50 + lat1 * 360000.0);
  int ilat2 = static_cast<int>(0.50 + lat2 * 360000.0);
  int ilon1 = static_cast<int>(0.50 + lon1 * 360000.0);
  int ilon2 = static_cast<int>(0.50 + lon2 * 360000.0);

  lat1 *= DEG2RAD;
  lon1 *= DEG2RAD;
  lat2 *= DEG2RAD;
  lon2 *= DEG2RAD;

  if ((ilat1 == ilat2) && (ilon1 == ilon2)) {
    return result;
  } else if (ilat1 == ilat2) {
    if (ilon1 < ilon2)
      result = 90.0;
    else
      result = 270.0;
  } else if (ilon1 == ilon2) {
    if (ilat1 > ilat2)
      result = 180.0;
  } else {
    double c = acos(sin(lat2)*sin(lat1) + cos(lat2)*cos(lat1)*cos((lon2-lon1)));
    double A = asin(cos(lat2)*sin((lon2-lon1))/sin(c));
    result = degs(A);

    if ((ilat2 > ilat1) && (ilon2 > ilon1)) {
    } else if ((ilat2 < ilat1) && (ilon2 < ilon1)) {
      result = 180.0 - result;
    } else if ((ilat2 < ilat1) && (ilon2 > ilon1)) {
      result = 180.0 - result;
    } else if ((ilat2 > ilat1) && (ilon2 < ilon1)) {
      result += 360.0;
    }
  }

  return result;
}

double RLIMath::ApproxDistance(double lat1, double lon1, double lat2, double lon2) {
  lat1 =  DEG2RAD * lat1;
  lon1 = -DEG2RAD * lon1;
  lat2 =  DEG2RAD * lat2;
  lon2 = -DEG2RAD * lon2;

  double F = (lat1 + lat2) / 2.0;
  double G = (lat1 - lat2) / 2.0;
  double L = (lon1 - lon2) / 2.0;

  double sing = sin(G);
  double cosl = cos(L);
  double cosf = cos(F);
  double sinl = sin(L);
  double sinf = sin(F);
  double cosg = cos(G);

  double S = sing*sing*cosl*cosl + cosf*cosf*sinl*sinl;
  double C = cosg*cosg*cosl*cosl + sinf*sinf*sinl*sinl;
  double W = atan2(sqrt(S),sqrt(C));
  double R = sqrt((S*C))/W;
  double H1 = (3 * R - 1.0) / (2.0 * C);
  double H2 = (3 * R + 1.0) / (2.0 * S);
  double D = 2 * W * ERADKM;
  return (D * (1 + FLATTENING * H1 * sinf*sinf*cosg*cosg - FLATTENING*H2*cosf*cosf*sing*sing));
}

double RLIMath::EllipsoidDistance(double lat1, double lon1, double lat2, double lon2) {
  double distance = 0.0;
  double faz, baz;
  double r = 1.0 - FLATTENING;
  double tu1, tu2, cu1, su1, cu2, x, sx, cx, sy, cy, y, sa, c2a, cz, e, c, d;
  double cosy1, cosy2;

  if(((lon1 - lon2) < EPS) && ((lat1 - lat2) < EPS))
    return distance;

  lon1 *= DEG2RAD;
  lon2 *= DEG2RAD;
  lat1 *= DEG2RAD;
  lat2 *= DEG2RAD;

  cosy1 = cos(lat1);
  cosy2 = cos(lat2);

  if(cosy1 == 0.0) cosy1 = 0.0000000001;
  if(cosy2 == 0.0) cosy2 = 0.0000000001;

  tu1 = r * sin(lat1) / cosy1;
  tu2 = r * sin(lat2) / cosy2;
  cu1 = 1.0 / sqrt(tu1 * tu1 + 1.0);
  su1 = cu1 * tu1;
  cu2 = 1.0 / sqrt(tu2 * tu2 + 1.0);
  x = lon2 - lon1;

  distance = cu1 * cu2;
  baz = distance * tu2;
  faz = baz * tu1;

  do {
    sx = sin(x);
    cx = cos(x);
    tu1 = cu2 * sx;
    tu2 = baz - su1 * cu2 * cx;
    sy = sqrt(tu1 * tu1 + tu2 * tu2);
    cy = distance * cx + faz;
    y = atan2(sy, cy);
    sa = distance * sx / sy;
    c2a = -sa * sa + 1.0;
    cz = faz + faz;
    if(c2a > 0.0) cz = -cz / c2a + cy;
    e = cz * cz * 2. - 1.0;
    c = ((-3.0 * c2a + 4.0) * FLATTENING + 4.0) * c2a * FLATTENING / 16.0;
    d = x;
    x = ((e * cy * c + cz) * sy * c + y) * sa;
    x = (1.0 - c) * x * FLATTENING + lon2 - lon1;
  } while(fabs(d - x) > EPS);

  x = sqrt((1.0 / r / r - 1.0) * c2a + 1.0) + 1.0;
  x = (x - 2.0) / x;
  c = 1.0 - x;
  c = (x * x / 4.0 + 1.0) / c;
  d = (0.375 * x * x - 1.0) * x;
  x = e * cy;
  distance = 1.0 - e - e;
  distance = ((((sy * sy * 4.0 - 3.0) *
  distance * cz * d / 6.0 - x) * d / 4.0 + cz) * sy * d + y) * c * ERADKM * r;

  return distance;
}
