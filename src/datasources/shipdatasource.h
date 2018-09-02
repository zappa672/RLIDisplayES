#ifndef SHIPDATASOURCE_H
#define SHIPDATASOURCE_H

#include <QObject>
#include <QVector2D>
#include <QDateTime>
#include <QTimerEvent>

class ShipDataSource : public QObject
{
  Q_OBJECT
public:
  explicit ShipDataSource(QObject *parent = 0);
  virtual ~ShipDataSource();

  inline const QVector2D& position() const { return _position; }

signals:
  void positionChanged(QVector2D position);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void start();
  void finish();

private:
  int _timerId;
  QDateTime _startTime;

  QVector2D _position;
};

#endif // SHIPDATASOURCE_H
