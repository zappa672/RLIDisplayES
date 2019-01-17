#ifndef SHIPDATASOURCE_H
#define SHIPDATASOURCE_H

#include <QObject>
#include <QVector2D>
#include <QDateTime>
#include <QTimerEvent>

#include "../common/rlimath.h"

struct RLIShipState {
  GeoPos position  { 0.0, 0.0 };
  double course    { 0.0 };
  double speed     { 0.0 };
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
