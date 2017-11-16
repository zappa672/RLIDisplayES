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
  nanosleep(&ts, NULL);
}

RadarDataSource::RadarDataSource() {
  finish_flag = true;
  _radar_scale = new RadarScale();

  _peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  _bearings_per_cycle  = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();

  file_amps[0] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps[1] = new GLfloat[_peleng_size*_bearings_per_cycle];

  loadData();
}

RadarDataSource::~RadarDataSource() {
  finish_flag = true;

  while(workerThread.isRunning());

  delete file_amps[0];
  delete file_amps[1];
}

void RadarDataSource::start() {
  if (workerThread.isRunning())
    return;

  finish_flag = false;
  workerThread = QtConcurrent::run(this, &RadarDataSource::worker);
}


void RadarDataSource::finish() {
  finish_flag = true;
}

void RadarDataSource::worker() {
  int delay = qApp->property(PROPERTY_DATA_DELAY).toInt();
  uint BLOCK_TO_SEND = qApp->property(PROPERTY_BLOCK_SIZE).toInt();
  int file = 0;
  int offset = 0;

  while(!finish_flag) {
    qSleep(delay);

    emit updateData(offset, BLOCK_TO_SEND, &file_amps[file][offset * _peleng_size]);
    emit updateData2(offset, BLOCK_TO_SEND, &file_amps[1 - file][offset * _peleng_size]);

    offset = (offset + BLOCK_TO_SEND) % _bearings_per_cycle;
    if (offset == 0) file = 1 - file;
  }
}


bool RadarDataSource::loadData() {
  if (!initWithDummy1(file_amps[0]))
    return false;

  if (!initWithDummy2(file_amps[1]))
    return false;

  return true;
}


bool RadarDataSource::initWithDummy1(GLfloat* amps) {
  for (uint i = 0; i < _bearings_per_cycle; i++)
    for (uint j = 0; j < _peleng_size; j++)
      amps[i*_peleng_size+j] = (255.f * j) / _peleng_size;

  return true;
}

bool RadarDataSource::initWithDummy2(GLfloat* amps) {
  for (uint i = 0; i < _bearings_per_cycle; i++)
    for (uint j = 0; j < _peleng_size; j++)
      amps[i*_peleng_size+j] = 255.f - (255.f * j) / _peleng_size;

  return true;
}
