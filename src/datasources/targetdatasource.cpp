#include "targetdatasource.h"

#include <qmath.h>


TargetDataSource::TargetDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  tail_mode    = TAILMODE_OFF;
  tail_minutes = 1;

  RadarTarget trgt;

  trgt.Lost = false;
  trgt.Latitude = 15.1200f;
  trgt.Longtitude = 145.6600f;
  trgt.Heading = 37.f;
  trgt.Rotation = 20.f;
  trgt.CourseOverGround = 37.f;
  trgt.SpeedOverGround = 180.f;

  _targets.push_back(trgt);

  trgt.Latitude = 15.1500f;
  trgt.Longtitude = 145.8600f;
  trgt.Heading = 123.f;
  trgt.Rotation = -20.f;
  trgt.CourseOverGround = 123.f;
  trgt.SpeedOverGround = 80.f;

  _targets.push_back(trgt);

  trgt.Latitude = 15.0500f;
  trgt.Longtitude = 145.4600f;
  trgt.Heading = 286.f;
  trgt.Rotation = 0.f;
  trgt.CourseOverGround = 286.f;
  trgt.SpeedOverGround = 140.f;

  _targets.push_back(trgt);

  trgt.Latitude = 12.3000f;
  trgt.Longtitude = -81.7300f;
  trgt.Heading = -1.f;
  trgt.Rotation = 0.f;
  trgt.CourseOverGround = 286.f;
  trgt.SpeedOverGround = 140.f;

  _targets.push_back(trgt);
}

TargetDataSource::~TargetDataSource() {
  finish();
}

void TargetDataSource::start() {
  if (_timerId != -1)
    return;

  _startTime = QDateTime::currentDateTime();
  _timerId = startTimer(20);
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
    RadarTarget target;

    target.Longtitude = _targets[i].Longtitude + 0.02f * (i+1) * sin(_startTime.msecsTo(now)/(2000.f*(i+1)) + i);
    target.Latitude = _targets[i].Latitude + 0.02f * (i+1) * cos(_startTime.msecsTo(now)/(2000.f*(i+1)) + i);
    target.CourseOverGround = int(450 + 180 * (_startTime.msecsTo(now)/(2000.f*(i+1)) + i) / PI) % 360;
    if (_targets[i].Heading != -1)
      target.Heading = int(360 + target.CourseOverGround - target.Rotation) % 360;
    else
      target.Heading = -1;
    target.SpeedOverGround = _targets[i].SpeedOverGround;

    emit updateTarget(QString::number(i+1), target);
  }
}

void TargetDataSource::incrementMode() {
  if (++tail_mode > TAILMODE_LAST)
    tail_mode = TAILMODE_FIRST;

  emit tailsModeChanged(tail_mode, (tail_mode == TAILMODE_DOTS) ? tail_minutes : 0);
}

void TargetDataSource::onTailsModeChanged(const QByteArray mode) {
  tail_minutes = atoi(mode.data());

  emit tailsModeChanged(tail_mode, (tail_mode == TAILMODE_DOTS) ? tail_minutes : 0);
}
