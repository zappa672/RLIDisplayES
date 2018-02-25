#include "shipdatasource.h"

#include <qmath.h>

ShipDataSource::ShipDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  //longtitude
  //position.first  = +12.5f;
  position.first  = 15.123f + 0.25f;
  //latitude
  //position.second = -81.6f;
  position.second = 145.66f;
}

ShipDataSource::~ShipDataSource() {
  finish();
}

void ShipDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();

  //longtitude
  position.first  = 15.123f + 0.25f * cos(_startTime.msecsTo(now)/30000.f);
  //latitude
  position.second = 145.66f + 0.25f * sin(_startTime.msecsTo(now)/30000.f);

  emit positionChanged(position);
}

void ShipDataSource::start() {
  if (_timerId != -1)
    return;

  _timerId = startTimer(1000);
  _startTime = QDateTime::currentDateTime();    
}

void ShipDataSource::finish() {
  if (_timerId == -1)
    return;

  killTimer(_timerId);
  _timerId = -1;
}
