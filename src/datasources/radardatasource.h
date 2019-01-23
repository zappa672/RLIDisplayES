#ifndef RADARDATASOURCE_H
#define RADARDATASOURCE_H

#include <stdint.h>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

#include <QtGlobal>
#include <QOpenGLFunctions>

class RadarDataSource : public QObject {
  Q_OBJECT
public:
  explicit RadarDataSource(QObject* parent = nullptr);
  virtual ~RadarDataSource();

public slots:
  void start();
  void finish();

signals:
  void updateRadarData(int offset, int count, GLfloat* amps);
  void updateTrailData(int offset, int count, GLfloat* amps);

protected slots:
  void timerEvent(QTimerEvent* e);

private:
  bool loadData();

  bool loadObserves1(char* filename, GLfloat* amps);

  bool initWithDummy1(float* amps);
  bool initWithDummy2(float* amps);
  bool initWithDummy3(float* amps);
  bool initWithDummy4(float* amps);

  int _timerId = -1;

  GLfloat* file_amps1[2];
  GLfloat* file_amps2[2];

  int _timer_period;
  int _blocks_to_send;

  int _file = 0;
  int _offset = 0;

  int _peleng_size;
  int _bearings_per_cycle;
};

#endif // RADARDATASOURCE_H

