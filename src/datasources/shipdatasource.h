#ifndef SHIPDATASOURCE_H
#define SHIPDATASOURCE_H

#include <QObject>
#include <QVector2D>
#include <QDateTime>
#include <QTimerEvent>

struct RLIShipState {
  QVector2D position { 0.f, 0.f };
  float course { 0.f };
  float speed { 0.f };
};


class ShipDataSource : public QObject
{
  Q_OBJECT
public:
  explicit ShipDataSource(QObject *parent = nullptr);
  virtual ~ShipDataSource();

  inline const RLIShipState& shipState() const { return _ship_state; }

signals:
  void shipStateChanged(RLIShipState state);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void start();
  void finish();

private:
  int _timerId = -1;
  QDateTime _startTime;  

  RLIShipState _ship_state;
};

#endif // SHIPDATASOURCE_H
