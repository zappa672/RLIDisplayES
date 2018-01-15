#ifndef SHIPDATASOURCE_H
#define SHIPDATASOURCE_H

#include <QObject>
#include <QDateTime>
#include <QTimerEvent>

class ShipDataSource : public QObject
{
  Q_OBJECT
public:
  explicit ShipDataSource(QObject *parent = 0);
  virtual ~ShipDataSource();

  inline std::pair<float, float> getPosition() { return position; }

signals:
  void positionChanged(std::pair<float, float> position);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void start();
  void finish();

private:
  int _timerId;
  QDateTime _startTime;

  std::pair<float, float> position;
};

#endif // SHIPDATASOURCE_H
