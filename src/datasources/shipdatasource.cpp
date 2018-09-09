#include "shipdatasource.h"

#include <qmath.h>

ShipDataSource::ShipDataSource(QObject *parent) : QObject(parent) {
  _timerId = -1;

  //longtitude
  _ship_state.position.setX( 15.123f + 0.25f );
  //latitude
  _ship_state.position.setY( 145.66f );
}

ShipDataSource::~ShipDataSource() {
  finish();
}

void ShipDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();

  //longtitude
  _ship_state.position.setX( 15.123f + 0.25f * cos(_startTime.msecsTo(now)/60000.f) );
  //latitude
  _ship_state.position.setY( 145.66f + 0.25f * sin(_startTime.msecsTo(now)/60000.f) );

  emit shipStateChanged(_ship_state);
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
