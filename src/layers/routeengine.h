#ifndef ROUTEENGINE_H
#define ROUTEENGINE_H

#include <QList>
#include <QMutex>
#include <QVector2D>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "../common/rlistate.h"

class RouteEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit RouteEngine(QOpenGLContext* context, QObject* parent = 0);
  virtual ~RouteEngine();

  void draw(const QMatrix4x4& mvp_matrix, const RLIState& state);

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
  void initShader();
  int loadBuffers();

  QMutex _routesMutex;
  QList<QVector2D> _currentRoute;
  QVector<QList<QVector2D> > _routes;

  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------
  enum { ROUTE_ATTR_PREV_COORDS = 0
       , ROUTE_ATTR_CURR_COORDS = 1
       , ROUTE_ATTR_NEXT_COORDS = 2
       , ROUTE_ATTR_COUNT       = 3 } ;

  enum { ROUTE_UNIF_MVP_MATRIX  = 0
       , ROUTE_UNIF_CENTER = 1
       , ROUTE_UNIF_SCALE = 2
       , ROUTE_UNIF_TYPE = 3
       , ROUTE_UNIF_COUNT = 4 } ;

  GLuint _vbo_ids[ROUTE_ATTR_COUNT];
  GLuint _attr_locs[ROUTE_ATTR_COUNT];
  GLuint _unif_locs[ROUTE_UNIF_COUNT];
};

#endif // ROUTEENGINE_H
