#include "chartmanager.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>

#include <QtConcurrentRun>

ChartManager::ChartManager(QObject *parent) : QObject(parent) {
  _s52_refs = new S52References(":/s52/chartsymbols.xml");
 // _s52_refs->print();
  _s52_refs->setColorScheme("DAY_BRIGHT");
}

ChartManager::~ChartManager() {
  for (S52::Chart* chart : _charts)
    delete chart;
}

void ChartManager::loadCharts() {
  QtConcurrent::run(this, &ChartManager::chartLoadingWorker);
}

void ChartManager::chartLoadingWorker() {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Charts loading started";

  QDir dir("data/charts");
  //dir.setNameFilters(QStringList("*.000"));
  dir.setNameFilters(QStringList("US2SP01M.000"));
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

  QStringList fileList = dir.entryList();
  for (int i = 0; i < fileList.count(); i++) {
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Loading: " << fileList[i];

    QString chart_path(dir.path() + "/" + fileList[i]);
    char* c_chart_path = new char[chart_path.size() + 1];
    for (int j = 0; j < chart_path.size(); j++)
      c_chart_path[j] = chart_path[j].toLatin1();
    c_chart_path[chart_path.size()] = '\0';

    S52::Chart* chart = new S52::Chart(c_chart_path, _s52_refs);

    _charts.insert(fileList[i], chart);
    delete[] c_chart_path;

    qDebug()  << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": "<< "Loaded: " << fileList[i];
    emit newChartAvailable(fileList[i]);
  }

  qDebug()  << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": "<< "Charts loading finished";
}
