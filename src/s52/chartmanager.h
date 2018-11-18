#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>
#include <QMap>

#include "s52chart.h"
#include "s52references.h"

class ChartManager : public QObject
{
  Q_OBJECT
public:
  explicit ChartManager(QObject *parent = nullptr);
  ~ChartManager();

  void loadCharts();

  inline S52References* refs() {return _s52_refs; }
  inline int chartCount() { return _charts.size(); }
  inline QList<QString> chartNames() { return _charts.keys(); }
  inline S52Chart* getChart(const QString& name) { if (!_charts.contains(name)) return nullptr; return _charts[name]; }

signals:
  void new_chart_available(const QString& name);

private:
  void chartLoadingWorker();

  QMap<QString, S52Chart*> _charts;
  S52References* _s52_refs;
};

#endif // CHARTMANAGER_H
