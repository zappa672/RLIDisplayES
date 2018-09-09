#ifndef TARGETDATASOURCE_H
#define TARGETDATASOURCE_H

#include <QObject>
#include <QVector>
#include <QVector2D>
#include <QDateTime>
#include <QTimerEvent>

struct RLITarget {
  RLITarget() { }

  RLITarget(const RLITarget& o) :
        lost        { o.lost },
        latitude    { o.latitude },
        longtitude  { o.longtitude },
        heading     { o.heading },
        rotation    { o.rotation },
        course_grnd { o.course_grnd },
        speed_grnd  { o.speed_grnd }
  { }

  ~RLITarget() { }

  bool lost         { false };
  float latitude    { 0.f };
  float longtitude  { 0.f };
  float heading     { 0.f };
  float rotation    { 0.f };
  float course_grnd { 0.f };
  float speed_grnd  { 0.f };
};

class TargetDataSource : public QObject
{
  Q_OBJECT
public:
  explicit TargetDataSource(QObject *parent = 0);
  virtual ~TargetDataSource();

signals:
  void updateTarget(const QString& tag, const RLITarget& target);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void start();
  void finish();

private:
  int _timerId;
  QDateTime _startTime;
  QVector<RLITarget> _targets;
};

#endif // TARGETDATASOURCE_H


/*
enum {
  TAILMODE_FIRST  = 0,
  TAILMODE_OFF    = 0,
  TAILMODE_RADAR  = 1,
  TAILMODE_DOTS   = 2,
  TAILMODE_LAST   = 2
};

void incrementMode();
void onTailsModeChanged(const QByteArray mode);
void tailsModeChanged(int mode, int minutes);
*/
