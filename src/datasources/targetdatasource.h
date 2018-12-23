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

  bool lost          { false };
  double latitude    { 0 };
  double longtitude  { 0 };
  double heading     { 0 };
  double rotation    { 0 };
  double course_grnd { 0 };
  double speed_grnd  { 0 };
};

class TargetDataSource : public QObject
{
  Q_OBJECT
public:
  explicit TargetDataSource(QObject *parent = nullptr);
  virtual ~TargetDataSource();

signals:
  void updateTarget(const QString& tag, const RLITarget& target);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void start();
  void finish();

private:
  int _timerId = -1;
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
