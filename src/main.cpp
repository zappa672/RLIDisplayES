#include "mainwindow.h"

#include <QApplication>
#include <QThreadPool>
#include <QGLFormat>
#include <QScreen>
#include <QSize>

#include "common/rlilayout.h"
#include "common/properties.h"

#define RLI_THREADS_NUM 6 // Required number of threads in global QThreadPool

void parseArgs(QApplication* a);


int main(int argc, char *argv[]) {
  if (QThreadPool::globalInstance()->maxThreadCount() < RLI_THREADS_NUM)
    QThreadPool::globalInstance()->setMaxThreadCount(RLI_THREADS_NUM);
  qDebug() << "Max number of threads: " << QThreadPool::globalInstance()->maxThreadCount();

  QApplication a(argc, argv);
  parseArgs(&a);

  QGLFormat format = QGLFormat::defaultFormat();
  format.setDoubleBuffer(true);
  format.setSampleBuffers(false);
  format.setSamples(1);
  QGLFormat::setDefaultFormat(format);

  MainWindow w;
  w.showFullScreen();

  return a.exec();
}

void parseArgs(QApplication* a) {
  QStringList args = a->arguments();

  if (args.contains("--help")) {
    qDebug() << "-bp to show debug buttons panel";
    qDebug() << "-p to setup peleng size (default: 800)";
    qDebug() << "-b to setup count of pelengs per circle (default: 4096)";
    qDebug() << "-f to setup delay between frames in milliseconds (default: 25)";
    qDebug() << "-d to setup delay between sending data blocks by radardatasource in milliseconds (default: 15)";
    qDebug() << "-s to setup size of data blocks to send in pelengs (default: 64)";
    qDebug() << "-w to setup rliwidget size (example: 1024x768, no default, depends on screen size)";
    exit(0);
  }

  a->setProperty(PROPERTY_SHOW_BUTTON_PANEL, args.contains("-bp"));

  a->setProperty(PROPERTY_PELENG_SIZE, args.contains("-p") ? args[args.indexOf("-p") + 1].toInt() : 800);
  a->setProperty(PROPERTY_BEARINGS_PER_CYCLE, args.contains("-b") ? args[args.indexOf("-b") + 1].toInt() : 4096);
  a->setProperty(PROPERTY_FRAME_DELAY, args.contains("-f") ? args[args.indexOf("-f") + 1].toInt() : 25);
  a->setProperty(PROPERTY_DATA_DELAY, args.contains("-d") ? args[args.indexOf("-d") + 1].toInt() : 30);
  a->setProperty(PROPERTY_BLOCK_SIZE, args.contains("-s") ? args[args.indexOf("-s") + 1].toInt() : 128);

  if (args.contains("-w"))
    a->setProperty(PROPERTY_RLI_WIDGET_SIZE, args[args.indexOf("-w") + 1]);
}
