#ifndef ROUTEENGINE_H
#define ROUTEENGINE_H

#include <QList>
#include <QMutex>
#include <QVector2D>
#include <QGLShaderProgram>
#include <QGLFunctions>

class RouteEngine : public QObject, protected QGLFunctions {
  Q_OBJECT
public:
  explicit RouteEngine(QObject* parent = 0);
  virtual ~RouteEngine();

  bool init(const QGLContext* context);
  void draw(QVector2D world_coords, float scale);

  QVector2D getLastPoint() { return _currentRoute.last(); }
  inline bool isIndexUsed(int index) { return (index < 0 || index >= _routes.size() || _routes[index].size() > 0); }

public slots:
  void clearCurrentRoute();
  void addPointToCurrent(const QVector2D& p);
  void removePointFromCurrent();

  void loadFrom(int index);
  void saveTo(int index);

protected slots:

private:
  bool _initialized;

  void initShader();
  int loadBuffers();

  QMutex _routesMutex;

  QGLShaderProgram* _prog;

  QList<QVector2D> _currentRoute;
  QVector<QList<QVector2D> > _routes;

  // -----------------------------------------------
  enum { ROUTE_ATTR_PREV_COORDS = 0
       , ROUTE_ATTR_CURR_COORDS = 1
       , ROUTE_ATTR_NEXT_COORDS = 2
       , ROUTE_ATTR_COUNT = 3 } ;
  enum { ROUTE_UNIF_CENTER = 0
       , ROUTE_UNIF_SCALE = 1
       , ROUTE_UNIF_TYPE = 2
       , ROUTE_UNIF_COUNT = 3 } ;

  GLuint _vbo_ids[ROUTE_ATTR_COUNT];
  GLuint _attr_locs[ROUTE_ATTR_COUNT];
  GLuint _unif_locs[ROUTE_UNIF_COUNT];
};

#endif // ROUTEENGINE_H
