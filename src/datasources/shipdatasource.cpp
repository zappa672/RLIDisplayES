#include "shipdatasource.h"

#include <cmath>

ShipDataSource::ShipDataSource(QObject *parent) : QObject(parent) {
  //_ship_state.position = GeoPos(13.23 + 0.25, 144.38 + 0.25);
  _ship_state.position = GeoPos(15.123 + 0.25, 145.66);
  _ship_state.course = 90;
  _ship_state.course = 0;
}

ShipDataSource::~ShipDataSource() {
  finish();
}

void ShipDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QDateTime now = QDateTime::currentDateTime();
  double elapsed = _startTime.msecsTo(now)/60000.;

  double lat = 15.123 + 0.25 * cos(elapsed);
  double lon = 145.66 + 0.25 * sin(elapsed);

  _ship_state.position = GeoPos(lat, lon);
  _ship_state.course = fmod(90 + RLIMath::degs(elapsed), 360);

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
