#include "mainwindow.h"

#include <QApplication>
#include <QThreadPool>
#include <QGLFormat>
#include <QScreen>
#include <QSize>

#include "common/rliconfig.h"

#define RLI_THREADS_NUM 6 // Required number of threads in global QThreadPool

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QStringList args = a.arguments();

  if (args.contains("--help")) {
    qDebug() << "-p to setup peleng size (default: 800)";
    qDebug() << "-b to setup count of pelengs per circle (default: 4096)";
    qDebug() << "-f to setup delay between frames in milliseconds (default: 25)";
    qDebug() << "-d to setup delay between sending data blocks by radardatasource in milliseconds (default: 15)";
    qDebug() << "-s to setup size of data blocks to send in pelengs (default: 64)";
    return 0;
  }


  if (args.contains("-p"))
    a.setProperty("PELENG_SIZE", args[args.indexOf("-p") + 1].toInt());
  else
    a.setProperty("PELENG_SIZE", 800);

  if (args.contains("-b"))
    a.setProperty("BEARINGS_PER_CYCLE", args[args.indexOf("-b") + 1].toInt());
  else
    a.setProperty("BEARINGS_PER_CYCLE", 4096);

  if (args.contains("-f"))
    a.setProperty("FRAME_DELAY", args[args.indexOf("-f") + 1].toInt());
  else
    a.setProperty("FRAME_DELAY", 25);

  if (args.contains("-d"))
    a.setProperty("DATA_DELAY", args[args.indexOf("-d") + 1].toInt());
  else
    a.setProperty("DATA_DELAY", 15);

  if (args.contains("-s"))
    a.setProperty("BLOCK_SIZE", args[args.indexOf("-s") + 1].toInt());
  else
    a.setProperty("BLOCK_SIZE", 64);



  QGLFormat format = QGLFormat::defaultFormat();
  format.setDoubleBuffer(true);
  format.setSampleBuffers(false);
  format.setSamples(1);

  QGLFormat::setDefaultFormat(format);

  if (QThreadPool::globalInstance()->maxThreadCount() < RLI_THREADS_NUM)
    QThreadPool::globalInstance()->setMaxThreadCount(RLI_THREADS_NUM);

  QScreen *screen = QGuiApplication::primaryScreen();
  bool showButtonPanel = RLIConfig::instance().showButtonPanel();
  QSize s = screen->geometry().size();
  QSize availableSize(showButtonPanel ? s.width() - 300 : s.width(), s.height());
  RLIConfig::instance().setCurrentSize(availableSize);

  MainWindow w;
  w.showFullScreen();

  return a.exec();
}
