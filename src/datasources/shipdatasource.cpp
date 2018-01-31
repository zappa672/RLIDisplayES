#include "shipdatasource.h"

#include <qmath.h>

ShipDataSource::ShipDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  //latitude
  position.second = -81.6f;
  //longtitude
  position.first  = +12.5f;
}

ShipDataSource::~ShipDataSource() {
  finish();
}

void ShipDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();

  //latitude
  position.second = -81.6f + 0.2f * sin(_startTime.msecsTo(now)/40000.f);
  //longtitude
  position.first  = +12.5f + 0.2f * cos(_startTime.msecsTo(now)/40000.f);

  emit positionChanged(position);
}

void ShipDataSource::start() {
  if (_timerId != -1)
    return;

  _timerId = startTimer(5000);
  _startTime = QDateTime::currentDateTime();    
}

void ShipDataSource::finish() {
  if (_timerId == -1)
    return;

  killTimer(_timerId);
  _timerId = -1;
}
