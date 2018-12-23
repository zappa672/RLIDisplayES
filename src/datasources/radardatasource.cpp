#include "radardatasource.h"
#include "../mainwindow.h"

#include "../common/properties.h"

#include <QApplication>
#include <QThread>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <QDebug>

void qSleep(int ms) {
  if (ms <= 0) return;
  struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
  nanosleep(&ts, nullptr);
}

RadarDataSource::RadarDataSource(QObject* parent) : QObject(parent) {
  //finish_flag = true;

  _peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  _bearings_per_cycle  = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();
  _timer_period        = qApp->property(PROPERTY_DATA_DELAY).toInt();
  _blocks_to_send      = qApp->property(PROPERTY_BLOCK_SIZE).toInt();

  file_amps1[0] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps1[1] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps2[0] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps2[1] = new GLfloat[_peleng_size*_bearings_per_cycle];

  loadData();
}

RadarDataSource::~RadarDataSource() {
  finish();

  delete file_amps1[0];
  delete file_amps1[1];
  delete file_amps2[0];
  delete file_amps2[1];
}

void RadarDataSource::start() {
  if (_timerId != -1)
    return;

  _timerId = startTimer(_timer_period);
}


void RadarDataSource::finish() {
  if (_timerId == -1)
    return;

  killTimer(_timerId);
  _timerId = -1;
}

void RadarDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e);

  QtConcurrent::run([&]() {
    emit updateRadarData(_offset, _blocks_to_send, &file_amps1[_file][_offset * _peleng_size]);
    emit updateTrailData(_offset, _blocks_to_send, &file_amps2[_file][_offset * _peleng_size]);
  });

  _offset = (_offset + _blocks_to_send) % _bearings_per_cycle;
  if (_offset == 0) _file = 1 - _file;
}



bool RadarDataSource::loadData() {
  if (!initWithDummy1(file_amps1[0]))
    return false;

  if (!initWithDummy2(file_amps1[1]))
    return false;

  if (!initWithDummy3(file_amps2[1]))
    return false;

  if (!initWithDummy4(file_amps2[0]))
    return false;

  return true;
}


bool RadarDataSource::initWithDummy1(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (i % 256 < 9 || i % 256 > 247)
        amps[i*_peleng_size+j] = (255.f * j) / _peleng_size;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}

bool RadarDataSource::initWithDummy2(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (j > 259 && j < 268)
        amps[i*_peleng_size+j] = 255.f - (255.f * i) / _bearings_per_cycle;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}

bool RadarDataSource::initWithDummy3(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (i % 256 < 137 && i % 256 > 121)
        amps[i*_peleng_size+j] = (255.f * j) / _peleng_size;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}

bool RadarDataSource::initWithDummy4(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (j > 131 && j < 140)
        amps[i*_peleng_size+j] = 255.f - (255.f * i) / _bearings_per_cycle;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}
