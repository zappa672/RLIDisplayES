#include "targetdatasource.h"

#include <qmath.h>


TargetDataSource::TargetDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  RLITarget trgt;

  trgt.lost = false;
  trgt.latitude = 15.4200f;
  trgt.longtitude = 145.7600f;
  trgt.heading = 37.f;
  trgt.rotation = 20.f;
  trgt.course_grnd = 37.f;
  trgt.speed_grnd = 180.f;

  _targets.push_back(trgt);

  trgt.latitude = 15.1500f;
  trgt.longtitude = 145.8600f;
  trgt.heading = 123.f;
  trgt.rotation = -20.f;
  trgt.course_grnd = 123.f;
  trgt.speed_grnd = 80.f;

  _targets.push_back(trgt);

  trgt.latitude = 15.3500f;
  trgt.longtitude = 145.5600f;
  trgt.heading = 286.f;
  trgt.rotation = 0.f;
  trgt.course_grnd = 286.f;
  trgt.speed_grnd = 140.f;

  _targets.push_back(trgt);

  trgt.latitude = 15.3000f;
  trgt.longtitude = 144.9300f;
  trgt.heading = -1.f;
  trgt.rotation = 0.f;
  trgt.course_grnd = 286.f;
  trgt.speed_grnd = 140.f;

  _targets.push_back(trgt);
}

TargetDataSource::~TargetDataSource() {
  finish();
}

void TargetDataSource::start() {
  if (_timerId != -1)
    return;

  _startTime = QDateTime::currentDateTime();
  _timerId = startTimer(200);
}

void TargetDataSource::finish() {
  if (_timerId == -1)
    return;

  killTimer(_timerId);
  _timerId = -1;
}

const float PI = 3.14159265359f;

void TargetDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();

  for (int i = 0; i < _targets.size(); i++) {
    RLITarget target;

    target.longtitude = _targets[i].longtitude + 0.02f * (i+1) * sin(_startTime.msecsTo(now)/(2000.f*(i+1)) + i);
    target.latitude = _targets[i].latitude + 0.02f * (i+1) * cos(_startTime.msecsTo(now)/(2000.f*(i+1)) + i);
    target.course_grnd = int(450 + 180 * (_startTime.msecsTo(now)/(2000.f*(i+1)) + i) / PI) % 360;
    if (_targets[i].heading != -1)
      target.heading = int(360 + target.course_grnd - target.rotation) % 360;
    else
      target.heading = -1;
    target.speed_grnd = _targets[i].speed_grnd;

    emit updateTarget(QString::number(i+1), target);
  }
}


/*
tail_mode    = TAILMODE_OFF;
tail_minutes = 1;

void TargetDataSource::incrementMode() {
  if (++tail_mode > TAILMODE_LAST)
    tail_mode = TAILMODE_FIRST;

  emit tailsModeChanged(tail_mode, (tail_mode == TAILMODE_DOTS) ? tail_minutes : 0);
}

void TargetDataSource::onTailsModeChanged(const QByteArray mode) {
  tail_minutes = atoi(mode.data());

  emit tailsModeChanged(tail_mode, (tail_mode == TAILMODE_DOTS) ? tail_minutes : 0);
}
*/
