#include "routeengine.h"

#include "../common/rlistate.h"
#include "../common/properties.h"

RouteEngine::RouteEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  _currentRoute.push_back(QVector2D(15.3730f, 145.6600f));
  _currentRoute.push_back(QVector2D(15.3640f, 145.7200f));
  _currentRoute.push_back(QVector2D(15.3340f, 145.8000f));
  _currentRoute.push_back(QVector2D(15.3040f, 145.8300f));
  _currentRoute.push_back(QVector2D(15.2440f, 145.8600f));

  initializeOpenGLFunctions();

  _prog = new QOpenGLShaderProgram();

  glGenBuffers(ROUTE_ATTR_COUNT, _vbo_ids);
  initShader();
}

RouteEngine::~RouteEngine() {
  delete _prog;
  glDeleteBuffers(ROUTE_ATTR_COUNT, _vbo_ids);
}

void RouteEngine::clearCurrentRoute() {
  _routesMutex.lock();

  _currentRoute.clear();

  _routesMutex.unlock();
}

void RouteEngine::addPointToCurrent(const QVector2D& p) {
  _routesMutex.lock();

  _currentRoute.push_back(p);

  _routesMutex.unlock();
}

void RouteEngine::removePointFromCurrent() {
  _routesMutex.lock();

  if (_currentRoute.size() > 1)
    _currentRoute.removeLast();

  _routesMutex.unlock();
}

void RouteEngine::loadFrom(int index) {
  _routesMutex.lock();

  if (index >= 0 && index < _routes.size())
    _currentRoute = _routes[index];

  _routesMutex.unlock();
}

void RouteEngine::saveTo(int index) {
  _routesMutex.lock();

  if (index >= 0 && index < _routes.size())
    _routes[index] = _currentRoute;

  _routesMutex.unlock();
}




void RouteEngine::draw(const QMatrix4x4& mvp_matrix, const RLIState& state) {
  _routesMutex.lock();

  _prog->bind();

  glUniform1f(_unif_locs[ROUTE_UNIF_SCALE], state.chart_scale);
  glUniform2f(_unif_locs[ROUTE_UNIF_CENTER], state.ship_position.first, state.ship_position.second);
  _prog->setUniformValue(_unif_locs[ROUTE_UNIF_MVP_MATRIX], mvp_matrix);

  int pCount = loadBuffers();

  glUniform1f(_unif_locs[ROUTE_UNIF_TYPE], 0);
  glPointSize(5);
  glDrawArrays(GL_POINTS, 0, pCount);


  glLineWidth(1);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);

  glUniform1f(_unif_locs[ROUTE_UNIF_TYPE], 1);
  glLineWidth(1);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);

  glUniform1f(_unif_locs[ROUTE_UNIF_TYPE], 2);
  glLineWidth(1);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);

  _prog->release();

  _routesMutex.unlock();
}

void RouteEngine::initShader() {
  // Overriding system locale until shaders are compiled
  setlocale(LC_NUMERIC, "C");

  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "route.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "route.frag.glsl");

  _prog->link();
  _prog->bind();

  _attr_locs[ROUTE_ATTR_PREV_COORDS] = _prog->attributeLocation("prev_world_coords");
  _attr_locs[ROUTE_ATTR_CURR_COORDS] = _prog->attributeLocation("curr_world_coords");
  _attr_locs[ROUTE_ATTR_NEXT_COORDS] = _prog->attributeLocation("next_world_coords");

  _unif_locs[ROUTE_UNIF_MVP_MATRIX] = _prog->uniformLocation("mvp_matrix");
  _unif_locs[ROUTE_UNIF_CENTER]     = _prog->uniformLocation("center");
  _unif_locs[ROUTE_UNIF_SCALE]      = _prog->uniformLocation("scale");
  _unif_locs[ROUTE_UNIF_TYPE]       = _prog->uniformLocation("type");

  _prog->release();

  // Restore system locale
  setlocale(LC_ALL, "");
}

int RouteEngine::loadBuffers() {
  std::vector<GLfloat> prev_points;
  std::vector<GLfloat> curr_points;
  std::vector<GLfloat> next_points;

  QList<QVector2D>::const_iterator it;
  for (it = _currentRoute.begin(); it != _currentRoute.end(); it++) {
    curr_points.push_back( (*it).x() );
    curr_points.push_back( (*it).y() );

    if (it == _currentRoute.begin()) {
      prev_points.push_back( (*it).x() );
      prev_points.push_back( (*it).y() );
    } else {
      prev_points.push_back( (*(it-1)).x() );
      prev_points.push_back( (*(it-1)).y() );
    }

    if ((it+1) == _currentRoute.end()) {
      next_points.push_back( (*it).x() );
      next_points.push_back( (*it).y() );
    } else {
      next_points.push_back( (*(it+1)).x() );
      next_points.push_back( (*(it+1)).y() );
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ROUTE_ATTR_PREV_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, prev_points.size()*sizeof(GLfloat), prev_points.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[ROUTE_ATTR_PREV_COORDS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[ROUTE_ATTR_PREV_COORDS]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ROUTE_ATTR_CURR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, curr_points.size()*sizeof(GLfloat), curr_points.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[ROUTE_ATTR_CURR_COORDS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[ROUTE_ATTR_CURR_COORDS]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ROUTE_ATTR_NEXT_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, next_points.size()*sizeof(GLfloat), next_points.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[ROUTE_ATTR_NEXT_COORDS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[ROUTE_ATTR_NEXT_COORDS]);

  return curr_points.size() / 2;
}
