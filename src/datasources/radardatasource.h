#ifndef RADARDATASOURCE_H
#define RADARDATASOURCE_H

#include "radarscale.h"

#include <stdint.h>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

#include <QtGlobal>
#include <QOpenGLFunctions>

class RadarDataSource : public QObject {
  Q_OBJECT
public:
  explicit RadarDataSource();
  virtual ~RadarDataSource();

public slots:
  void start();
  void finish();

signals:
  void updateData(uint offset, uint count, GLfloat* amps);
  void updateData2(uint offset, uint count, GLfloat* amps);

private:
  bool loadData();
  bool initWithDummy1(float* amps);
  bool initWithDummy2(float* amps);

  bool finish_flag;
  GLfloat* file_amps[2];
  uint  file_curr;

  RadarScale* _radar_scale;

  uint _peleng_size;
  uint _bearings_per_cycle;

  void worker();
  QFuture<void> workerThread;
};

#endif // RADARDATASOURCE_H

