#include "shipdatasource.h"

#include <qmath.h>

ShipDataSource::ShipDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  //latitude
  position.first = -81.6000f;
  //longtitude
  position.second = 12.7000f;
}

ShipDataSource::~ShipDataSource() {
  finish();
}

void ShipDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();

  //latitude
  position.first =  12.5000f + 0.2f * cos(_startTime.msecsTo(now)/400000.f);
  //longtitude
  position.second =  -81.6000f + 0.2f * sin(_startTime.msecsTo(now)/400000.f);

  emit positionChanged(position);
}

void ShipDataSource::start() {
  if (_timerId != -1)
    return;

  _startTime = QDateTime::currentDateTime();  
  _timerId = startTimer(500);
}

void ShipDataSource::finish() {
  if (_timerId == -1)
    return;

  killTimer(_timerId);
  _timerId = -1;

}
