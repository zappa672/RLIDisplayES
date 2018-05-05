#ifndef TARGETDATASOURCE_H
#define TARGETDATASOURCE_H

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QTimerEvent>

struct RadarTarget {
public:
  RadarTarget() {
    Lost = false;
    Latitude = 0;
    Longtitude = 0;
    Heading = 0;
    Rotation = 0;
    CourseOverGround = 0;
    SpeedOverGround = 0;
  }

  RadarTarget(const RadarTarget& o) {
    Lost = o.Lost;
    Latitude = o.Latitude;
    Longtitude = o.Longtitude;
    Heading = o.Heading;
    Rotation = o.Rotation;
    CourseOverGround = o.CourseOverGround;
    SpeedOverGround = o.SpeedOverGround;
  }

  ~RadarTarget() { }

  bool Lost;
  float Latitude, Longtitude;
  float Heading, Rotation;
  float CourseOverGround, SpeedOverGround;
};

Q_DECLARE_METATYPE(RadarTarget)


class TargetDataSource : public QObject
{
  Q_OBJECT
public:
  explicit TargetDataSource(QObject *parent = 0);
  virtual ~TargetDataSource();

  enum {
    TAILMODE_FIRST  = 0,
    TAILMODE_OFF    = 0,
    TAILMODE_RADAR  = 1,
    TAILMODE_DOTS   = 2,
    TAILMODE_LAST   = 2
  };

  void incrementMode();

signals:
  void updateTarget(QString tag, RadarTarget target);
  void tailsModeChanged(int mode, int minutes);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void start();
  void finish();

  void onTailsModeChanged(const QByteArray mode);

private:
  int _timerId;
  QDateTime _startTime;

  int tail_mode;
  int tail_minutes;

  QVector<RadarTarget> _targets;
};

#endif // TARGETDATASOURCE_H
