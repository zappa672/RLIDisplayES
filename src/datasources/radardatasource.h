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
  void updateData(int offset, int count, GLfloat* amps);
  void updateData2(int offset, int count, GLfloat* amps);

private:
  bool loadData();
  bool initWithDummy1(float* amps);
  bool initWithDummy2(float* amps);
  bool initWithDummy3(float* amps);
  bool initWithDummy4(float* amps);

  bool finish_flag;
  GLfloat* file_amps1[2];
  GLfloat* file_amps2[2];
  int  file_curr;

  int _peleng_size;
  int _bearings_per_cycle;

  void worker();
  QFuture<void> workerThread;
};

#endif // RADARDATASOURCE_H

