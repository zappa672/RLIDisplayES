#include "routeengine.h"

#include "../common/rlistate.h"
#include "../common/properties.h"

RouteEngine::RouteEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  _currentRoute.push_back(QPointF(15.3730f, 145.6600f));
  _currentRoute.push_back(QPointF(15.3640f, 145.7200f));
  _currentRoute.push_back(QPointF(15.3340f, 145.8000f));
  _currentRoute.push_back(QPointF(15.3040f, 145.8300f));
  _currentRoute.push_back(QPointF(15.2440f, 145.8600f));

  initializeOpenGLFunctions();

  _prog = new QOpenGLShaderProgram();

  glGenBuffers(ROUTE_ATTR_COUNT, _vbo_ids[0]);
  glGenBuffers(ROUTE_ATTR_COUNT, _vbo_ids[1]);
  glGenBuffers(ROUTE_ATTR_COUNT, _vbo_ids[2]);
  initShader();
  loadBuffers();
}

RouteEngine::~RouteEngine() {
  delete _prog;
  glDeleteBuffers(ROUTE_ATTR_COUNT, _vbo_ids[0]);
  glDeleteBuffers(ROUTE_ATTR_COUNT, _vbo_ids[1]);
  glDeleteBuffers(ROUTE_ATTR_COUNT, _vbo_ids[2]);
}

void RouteEngine::clearCurrentRoute() {
  _routesMutex.lock();

  _currentRoute.clear();

  _routesMutex.unlock();
}

void RouteEngine::addPointToCurrent(const QPointF& p) {
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

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[0][ROUTE_ATTR_COORDS]);
  glVertexAttribPointer(_attr_locs[ROUTE_ATTR_COORDS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[ROUTE_ATTR_COORDS]);

  int pCount = _currentRoute.size();

#if !(defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0))
  glPointSize(5.0);
#endif
  glDrawArrays(GL_POINTS, 0, pCount);

  glLineWidth(1.0);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);


  /*
  glUniform1f(_unif_locs[ROUTE_UNIF_TYPE], 1);
  glLineWidth(1.0);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);

  glUniform1f(_unif_locs[ROUTE_UNIF_TYPE], 2);
  glLineWidth(1.0);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);
  */

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

  _attr_locs[ROUTE_ATTR_COORDS] = _prog->attributeLocation("world_coords");

  _unif_locs[ROUTE_UNIF_MVP_MATRIX] = _prog->uniformLocation("mvp_matrix");
  _unif_locs[ROUTE_UNIF_CENTER]     = _prog->uniformLocation("center");
  _unif_locs[ROUTE_UNIF_SCALE]      = _prog->uniformLocation("scale");

  _prog->release();

  // Restore system locale
  setlocale(LC_ALL, "");
}

int RouteEngine::loadBuffers() {
  std::vector<GLfloat> ps1, ps2, ps3;

  for (int i = 0; i < _currentRoute.size(); i++) {
    QPointF p = _currentRoute[i];

    ps1.push_back( p.x() );
    ps1.push_back( p.y() );


    QVector2D norm1 { 0.f, 0.f };
    QVector2D norm2 { 0.f, 0.f };
    QVector2D norm { 0.f, 0.f };

    if (i > 0) {
      QPointF pp = _currentRoute[i-1];
      norm1 = QVector2D(p.y() - pp.y(), pp.x() - p.x());
      norm1.normalize();
    }

    if (i < _currentRoute.size()-1) {
      QPointF pp = _currentRoute[i+1];
      norm2 = QVector2D(p.y() - pp.y(), pp.x() - p.x());
      norm2.normalize();
    }

    if (i == 0 && _currentRoute.size() > 0)
      norm = norm2;
    else if (i == _currentRoute.size()-1 && _currentRoute.size() > 1)
      norm = norm1;
    else norm = (norm1 + norm2) / 2.f;


    ps2.push_back( p.x() + norm.x() );
    ps2.push_back( p.y() + norm.y() );

    ps3.push_back( p.x() - norm.x() );
    ps3.push_back( p.y() - norm.y() );
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[0][ROUTE_ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, ps1.size()*sizeof(GLfloat), ps1.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[1][ROUTE_ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, ps2.size()*sizeof(GLfloat), ps2.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[2][ROUTE_ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, ps3.size()*sizeof(GLfloat), ps3.data(), GL_DYNAMIC_DRAW);

  return ps1.size() / 2;
}
