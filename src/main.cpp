#include "mainwindow.h"

#include <QApplication>
#include <QThreadPool>
#include <QGLFormat>
#include <QScreen>
#include <QSize>

#include "common/rliconfig.h"
#include "common/properties.h"

#define RLI_THREADS_NUM 6 // Required number of threads in global QThreadPool



int main(int argc, char *argv[]) {
  if (QThreadPool::globalInstance()->maxThreadCount() < RLI_THREADS_NUM)
    QThreadPool::globalInstance()->setMaxThreadCount(RLI_THREADS_NUM);
  qDebug() << "Max number of threads: " << QThreadPool::globalInstance()->maxThreadCount();

  QApplication a(argc, argv);

  QStringList args = a.arguments();

  if (args.contains("--help")) {
    qDebug() << "-p to setup peleng size (default: 800)";
    qDebug() << "-b to setup count of pelengs per circle (default: 4096)";
    qDebug() << "-f to setup delay between frames in milliseconds (default: 50)";
    qDebug() << "-d to setup delay between sending data blocks by radardatasource in milliseconds (default: 15)";
    qDebug() << "-s to setup size of data blocks to send in pelengs (default: 64)";
    qDebug() << "-w to setup rliwidget size (example: 1024x768, no default, depends on screen size)";
    return 0;
  }


  if (args.contains("-p"))
    a.setProperty(PROPERTY_PELENG_SIZE, args[args.indexOf("-p") + 1].toInt());
  else
    a.setProperty(PROPERTY_PELENG_SIZE, 800);

  if (args.contains("-b"))
    a.setProperty(PROPERTY_BEARINGS_PER_CYCLE, args[args.indexOf("-b") + 1].toInt());
  else
    a.setProperty(PROPERTY_BEARINGS_PER_CYCLE, 4096);

  if (args.contains("-f"))
    a.setProperty(PROPERTY_FRAME_DELAY, args[args.indexOf("-f") + 1].toInt());
  else
    a.setProperty(PROPERTY_FRAME_DELAY, 50);

  if (args.contains("-d"))
    a.setProperty(PROPERTY_DATA_DELAY, args[args.indexOf("-d") + 1].toInt());
  else
    a.setProperty(PROPERTY_DATA_DELAY, 30);

  if (args.contains("-s"))
    a.setProperty(PROPERTY_BLOCK_SIZE, args[args.indexOf("-s") + 1].toInt());
  else
    a.setProperty(PROPERTY_BLOCK_SIZE, 128);

  if (args.contains("-w"))
    a.setProperty(PROPERTY_RLI_WIDGET_SIZE, args[args.indexOf("-w") + 1]);


  QGLFormat format = QGLFormat::defaultFormat();
  format.setDoubleBuffer(true);
  format.setSampleBuffers(false);
  format.setSamples(1);
  QGLFormat::setDefaultFormat(format);


  MainWindow w;
  w.showFullScreen();

  return a.exec();
}
