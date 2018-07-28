#include "targetengine.h"

#include "../common/properties.h"
#include "../common/rlimath.h"

#include <QImage>
#include <QDateTime>


TargetEngine::TargetEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  _tailsTime   = 0;
  _selected = "1";

  connect(&_tailsTimer, SIGNAL(timeout()), SLOT(onTailsTimer()));
  if (_tailsTime > 0)
    _tailsTimer.start((_tailsTime * 60 * 1000) / TRG_TAIL_NUM);


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
  delete _asset_tex;
  delete _selection_tex;

  glDeleteBuffers(AIS_TRGT_ATTR_COUNT, _vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
}

int TargetEngine::getCurrentIndex() {
  if (_targets.contains(_selected))
    return _targets.keys().indexOf(_selected);
  else
    return -1;
}

void TargetEngine::trySelect(QVector2D cursorCoords, float scale) {
  QList<QString> tags = _targets.keys();
  for (int i = 0; i < tags.size(); i++) {
    if (tags[i] == _selected)
      continue;

    QVector2D target_coords(_targets[tags[i]].Latitude, _targets[tags[i]].Longtitude);
    QPointF dist_to_target = RLIMath::coords_to_pos(cursorCoords, target_coords, QPoint(0, 0), scale);
    if (QVector2D(dist_to_target).length() < 16) {
      _selected = tags[i];
      emit selectedTargetUpdated(_selected, _targets[_selected]);
      return;
    }
  }
}


void TargetEngine::onTailsTimer() {
  _trgtsMutex.lock();
  //qDebug() << QDateTime::currentDateTime() << ": " << "onTailsTimer";

  QList<QString> tags = _targets.keys();
  for (int i = 0; i < tags.count(); i++) {
    QString tag = tags[i];
    _tails[tag].push_back(QVector2D(_targets[tag].Latitude,_targets[tag].Longtitude));

    if (_tails[tag].size() > TRG_TAIL_NUM)
      _tails[tag].removeFirst();
  }

  _trgtsMutex.unlock();
  //qDebug() << _tails[tags[0]].size();
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
      _tails[tag].push_back(QVector2D(_targets[tag].Latitude,_targets[tag].Longtitude));

      if (_tails[tag].size() > TRG_TAIL_NUM)
        _tails[tag].removeFirst();
    }

    _trgtsMutex.unlock();
  }
  else
    _tailsTimer.start((_tailsTime * 60 * 1000) / TRG_TAIL_NUM);
}

void TargetEngine::updateTarget(QString tag, RadarTarget target) {
  _trgtsMutex.lock();

  if (!_targets.contains(tag)) {
    _targets.insert(tag, target);
    _tails.insert(tag, QList<QVector2D>());    
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
      emit selectedTargetUpdated(tag, RadarTarget());
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

  auto coords = state.shipPosition();
  glUniform1f(_unif_locs[AIS_TRGT_UNIF_SCALE], state.chartScale());
  glUniform2f(_unif_locs[AIS_TRGT_UNIF_CENTER], coords.first, coords.second);
  _prog->setUniformValue(_unif_locs[AIS_TRGT_UNIF_MVP], mvp_matrix);

  initBuffersTrgts("");
  bindBuffers();

  glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 0);


  // Draw target marks
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _asset_tex->textureId());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glDrawElements(GL_TRIANGLES, 6*_targets.size(), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);



  glLineWidth(2);

  // Draw target headings
  glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 1);
  glDrawArrays(GL_LINES, 0,  _targets.size()*4);

  // Draw target courses
  // glPushAttrib(GL_ENABLE_BIT);
  // glLineStipple(1, 0xF0F0);
  // glEnable(GL_LINE_STIPPLE);

  glUniform1f(_unif_locs[AIS_TRGT_UNIF_TYPE], 2);
  glDrawArrays(GL_LINES, 0, _targets.size()*4);
  // glPopAttrib();

  // glPointSize(5);


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
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_COORDS], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_COORDS]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ORDER]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_HEADING]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_HEADING], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_HEADING]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ROTATION]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_ROTATION], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_ROTATION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COURSE]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_COURSE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[AIS_TRGT_ATTR_COURSE]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_SPEED]);
  glVertexAttribPointer(_attr_locs[AIS_TRGT_ATTR_SPEED], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
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
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_HEADING]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_ROTATION]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_COURSE]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AIS_TRGT_ATTR_SPEED]);
  glBufferData(GL_ARRAY_BUFFER, point_count*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

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
        point.push_back(_targets[keys[trgt]].Latitude);
        point.push_back(_targets[keys[trgt]].Longtitude);
        heading.push_back(_targets[keys[trgt]].Heading);
        rotation.push_back(_targets[keys[trgt]].Rotation);
        course.push_back(_targets[keys[trgt]].CourseOverGround);
        speed.push_back(_targets[keys[trgt]].SpeedOverGround);
      }
    }
  } else {
    for (int i = 0; i < 4; i++) {
      order.push_back(i);
      point.push_back(_targets[tag].Latitude);
      point.push_back(_targets[tag].Longtitude);
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

  /*
  glGenTextures(1, tex_id);

  glBindTexture(GL_TEXTURE_2D, *tex_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  QImage img(path);
  img = QOpenGLContext::convertToGLFormat(img);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height()
               , 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  */
}

int TargetEngine::getTailsTime(void) {
    return _tailsTime;
}
