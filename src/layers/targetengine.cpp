#include "targetengine.h"

#include "../common/properties.h"
#include "../common/rlimath.h"

#include <QImage>
#include <QDateTime>


TargetEngine::TargetEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  _tailsTime = 1;
  _selected = "";

  startTimer(2000);

  initializeOpenGLFunctions();

  _prog = new QOpenGLShaderProgram();

  glGenBuffers(1, &_ind_vbo_id);
  glGenBuffers(AIS_TRGT_ATTR_COUNT, _vbo_ids);

  initShader();

  _asset_tex = initTexture("data//textures//targets///target.png");
  _selection_tex = initTexture("data//textures//targets//selection.png");
}

TargetEngine::~TargetEngine() {
  delete _prog;
  //delete _asset_tex;
  //delete _selection_tex;

  glDeleteBuffers(AIS_TRGT_ATTR_COUNT, _vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
}

void TargetEngine::select(const GeoPos& coords, double scale) {
  for (const QString& tag: _targets.keys()) {
    if (tag == _selected)
      continue;

    float dist = RLIMath::GCDistance(coords.lat, coords.lon, _targets[tag].latitude, _targets[tag].longtitude);

    if ((1000 * dist / scale) < 32) {
      _selected = tag;
      emit selectedTargetUpdated(_selected, _targets[_selected]);
      return;
    }
  }
}


void TargetEngine::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  _trgtsMutex.lock();

  QList<QString> tags = _targets.keys();
  for (int i = 0; i < tags.count(); i++) {
    QString tag = tags[i];
    _tails[tag].push_back(QVector2D(_targets[tag].latitude,_targets[tag].longtitude));

    while (_tails[tag].size() > TRG_TAIL_NUM)
      _tails[tag].removeFirst();
  }

  _trgtsMutex.unlock();
}

void TargetEngine::onTailsModeChanged(int mode, int minutes) {
  Q_UNUSED(mode);

  if (_tailsTimer.isActive())
      _tailsTimer.stop();

  _tailsTime = minutes;

  if(_tailsTime <= 0) {
    _trgtsMutex.lock();
    //qDebug() << QDateTime::currentDateTime() << ": " << "onTailsTimer";

    QList<QString> tags = _targets.keys();
    for (int i = 0; i < tags.count(); i++) {
      QString tag = tags[i];
      _tails[tag].push_back(QVector2D(_targets[tag].latitude,_targets[tag].longtitude));

      if (_tails[tag].size() > TRG_TAIL_NUM)
        _tails[tag].removeFirst();
    }

    _trgtsMutex.unlock();
  }
  else
    _tailsTimer.start((_tailsTime * 60 * 1000) / TRG_TAIL_NUM);
}

void TargetEngine::updateTarget(QString tag, RLITarget target) {
  _trgtsMutex.lock();

  if (!_targets.contains(tag)) {
    _targets.insert(tag, target);
    _tails.insert(tag, QList<QVector2D>());
    _selected = tag;
    emit targetCountChanged(_targets.size());
  } else {
    _targets[tag] = target;
    if (tag == _selected)
      emit selectedTargetUpdated(tag, target);
  }

  _trgtsMutex.unlock();
}


void TargetEngine::deleteTarget(QString tag) {
  _trgtsMutex.lock();

  if (_targets.contains(tag)) {
    _targets.remove(tag);
    _tails.remove(tag);
    emit targetCountChanged(_targets.size());

    if (tag == _selected) {
      emit selectedTargetUpdated(tag, RLITarget());
      _selected = "";
    }
  }

  _trgtsMutex.unlock();
}


void TargetEngine::initShader() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "trgt.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "trgt.frag.glsl");

  _prog->link();
  _prog->bind();

  _attr_locs[AIS_TRGT_ATTR_COORDS] = _prog->attributeLocation("world_coords");
  _attr_locs[AIS_TRGT_ATTR_ORDER] = _prog->attributeLocation("vertex_order");
  _attr_locs[AIS_TRGT_ATTR_HEADING] = _prog->attributeLocation("heading");
  _attr_locs[AIS_TRGT_ATTR_ROTATION] = _prog->attributeLocation("rotation");
  _attr_locs[AIS_TRGT_ATTR_COURSE] = _prog->attributeLocation("course");
  _attr_locs[AIS_TRGT_ATTR_SPEED] = _prog->attributeLocation("speed");

  _unif_locs[AIS_TRGT_UNIF_MVP] = _prog->uniformLocation("mvp_matrix");
  _unif_locs[AIS_TRGT_UNIF_CENTER] = _prog->uniformLocation("center");
  _unif_locs[AIS_TRGT_UNIF_SCALE] = _prog->uniformLocation("scale");
  _unif_locs[AIS_TRGT_UNIF_TYPE] = _prog->uniformLocation("type");

  _prog->release();
}


void TargetEngine::draw(const QMatrix4x4& mvp_matrix, const RLIState& state) {
  _trgtsMutex.lock();

  _prog->bind();

  auto coords = state.ship_position;
  glUniform1f(_unif_locs[AIS_TRGT_UNIF_SCALE], state.chart_scale);
  glUniform2f(_unif_locs[AIS_TRGT_UNIF_CENTER], coords.lat, coords.lon);
  _prog->setUniformValue(_unif_locs[AIS_TRGT_UNIF_MVP], mvp_matrix);

  initBuffersTrgts("");
  bindBuffers();

  glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 0);


  // Draw target marks
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _asset_tex->textureId());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glDrawElements(GL_TRIANGLES, 6*_targets.size(), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);

  glLineWidth(2);

  // Draw target headings
  glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 1);
  glDrawArrays(GL_LINES, 0,  _targets.size()*4);

  glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 2);
  glDrawArrays(GL_LINES, 0, _targets.size()*4);


#if !(defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0))
  glPointSize(5.0);
#endif


  // Draw tails++
  if (_tailsTime) {
    int pCount = initBuffersTails();
    bindBuffers();
    glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 3);
    glDrawArrays(GL_POINTS, 0, pCount);
  }


  if (_selected != "" && _targets.contains(_selected)) {
    initBuffersTrgts(_selected);
    bindBuffers();

    glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _selection_tex->textureId());
    glDrawArrays(GL_TRIANGLE_FAN, 0,  4);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  _prog->release();

  _trgtsMutex.unlock();
}


void TargetEngine::bindBuffers() {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COORDS]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_COORDS], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_COORDS]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ORDER]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_HEADING]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_HEADING], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_HEADING]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ROTATION]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_ROTATION], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_ROTATION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COURSE]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_COURSE], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_COURSE]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_SPEED]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_SPEED], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_SPEED]);
}


int TargetEngine::initBuffersTails() {
  std::vector<GLfloat> point;

  QList<QString> keys = _tails.keys();

  for (int trgt = 0; trgt < keys.size(); trgt++) {
    for (int i = 0; i < _tails[keys[trgt]].size(); i++) {
      point.push_back(_tails[keys[trgt]][i].x());
      point.push_back(_tails[keys[trgt]][i].y());
    }
  }

  int point_count = point.size()/2;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, point_count*2*sizeof(GLfloat), point.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_HEADING]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ROTATION]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COURSE]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_SPEED]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  return point_count;
}


void TargetEngine::initBuffersTrgts(QString tag) {
  std::vector<GLfloat> point, order, heading, rotation, course, speed;
  std::vector<GLuint> draw_indices;

  QList<QString> keys = _targets.keys();

  if (tag == "") {
    for (int trgt = 0; trgt < keys.size(); trgt++) {
      for (int i = 0; i < 4; i++) {
        order.push_back(i);
        point.push_back(_targets[keys[trgt]].latitude);
        point.push_back(_targets[keys[trgt]].longtitude);
        heading.push_back(_targets[keys[trgt]].heading);
        rotation.push_back(_targets[keys[trgt]].rotation);
        course.push_back(_targets[keys[trgt]].course_grnd);
        speed.push_back(_targets[keys[trgt]].speed_grnd);
      }
    }
  } else {
    for (int i = 0; i < 4; i++) {
      order.push_back(i);
      point.push_back(_targets[tag].latitude);
      point.push_back(_targets[tag].longtitude);
      heading.push_back(0);
      rotation.push_back(0);
      course.push_back(0);
      speed.push_back(0);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, point.size()*sizeof(GLfloat), point.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, order.size()*sizeof(GLfloat), order.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_HEADING]);
  glBufferData(GL_ARRAY_BUFFER, heading.size()*sizeof(GLfloat), heading.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ROTATION]);
  glBufferData(GL_ARRAY_BUFFER, rotation.size()*sizeof(GLfloat), rotation.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COURSE]);
  glBufferData(GL_ARRAY_BUFFER, course.size()*sizeof(GLfloat), course.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_SPEED]);
  glBufferData(GL_ARRAY_BUFFER, speed.size()*sizeof(GLfloat), speed.data(), GL_DYNAMIC_DRAW);


  for (int i = 0; i < _targets.size(); i++) {
    draw_indices.push_back(4*i);
    draw_indices.push_back(4*i+1);
    draw_indices.push_back(4*i+2);
    draw_indices.push_back(4*i);
    draw_indices.push_back(4*i+2);
    draw_indices.push_back(4*i+3);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*_targets.size()*sizeof(GLuint), draw_indices.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

QOpenGLTexture* TargetEngine::initTexture(QString path) {
  QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
  QImage img(path);

  tex->setMipLevels(1);
  tex->setMinificationFilter(QOpenGLTexture::Nearest);
  tex->setMagnificationFilter(QOpenGLTexture::Nearest);
  tex->setWrapMode(QOpenGLTexture::Repeat);

  tex->setData(img, QOpenGLTexture::DontGenerateMipMaps);

  return tex;
}

