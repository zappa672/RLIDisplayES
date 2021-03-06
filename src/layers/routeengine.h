#ifndef ROUTEENGINE_H
#define ROUTEENGINE_H

#define ROUTES_COUNT 4


#include <QList>
#include <QMutex>
#include <QVector2D>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "../common/rlistate.h"

class RouteEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit RouteEngine(QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~RouteEngine();

  void draw(const QMatrix4x4& mvp_matrix, const RLIState& state);

  GeoPos getLastPoint() { return _currentRoute.last(); }
  inline bool isIndexUsed(int index) { return (index < 0 || index >= 4 || _routes[index].size() > 0); }

public slots:
  void clearCurrentRoute();
  void addPointToCurrent(const GeoPos& p);
  void removePointFromCurrent();

  void loadFrom(int index);
  void saveTo(int index);

protected slots:

private:
  void initShader();
  int loadBuffers();

  QVector2D lineIntersection( const QVector2D& p11, const QVector2D& p12
                            , const QVector2D& p21, const QVector2D& p22) const;

  QMutex _routesMutex;
  QList<GeoPos> _currentRoute;
  bool _need_reload_buffer = false;
  QList<GeoPos> _routes[ROUTES_COUNT];

  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------
  enum { ROUTE_ATTR_COORDS  = 0
       , ROUTE_ATTR_COUNT   = 1 } ;

  enum { ROUTE_UNIF_MVP_MATRIX  = 0
       , ROUTE_UNIF_CENTER      = 1
       , ROUTE_UNIF_SCALE       = 2
       , ROUTE_UNIF_COUNT       = 3} ;

  GLuint _vbo_ids[3][ROUTE_ATTR_COUNT];
  GLuint _attr_locs[ROUTE_ATTR_COUNT];
  GLuint _unif_locs[ROUTE_UNIF_COUNT];
};

#endif // ROUTEENGINE_H
