#include "shipdatasource.h"

#include <qmath.h>

ShipDataSource::ShipDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  //latitude
  position.second = 145.9451f;
  //longtitude
  position.first = 15.3642f;
}

ShipDataSource::~ShipDataSource() {
  finish();
}

void ShipDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();

  //latitude
  position.first  = 145.9451f + 0.2f * cos(_startTime.msecsTo(now)/400000.f);
  //longtitude
  position.second = 15.3642f  + 0.2f * sin(_startTime.msecsTo(now)/400000.f);

  emit positionChanged(position);
}

void ShipDataSource::start() {
  if (_timerId != -1)
    return;

  _timerId = startTimer(500);
  _startTime = QDateTime::currentDateTime();    
}

void ShipDataSource::finish() {
  if (_timerId == -1)
    return;

  killTimer(_timerId);
  _timerId = -1;
}
