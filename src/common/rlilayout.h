#ifndef RLILAYOUT_H
#define RLILAYOUT_H

#include <QString>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QVector>
#include <QMap>

struct RLIPanelTableInfo {
  QMap<QString, QString> params;
  QVector<QMap<QString, QString>> columns;

  inline void clear() {
    params.clear();
    columns.clear();
  }
};

struct RLIPanelInfo {
  QSize size;
  QPoint position;

  QMap<QString, QString> params;

  QMap<QString, QMap<QString, QString>> texts;
  QMap<QString, QMap<QString, QString>> rects;
  QMap<QString, RLIPanelTableInfo> tables;

  inline void clear() {
    params.clear();
    texts.clear();
    rects.clear();
    tables.clear();
  }
};

struct RLICircleInfo {
  QPoint center;
  int radius;
  QRect boundRect;
  QString font;
};

struct RLILayout {
  RLICircleInfo circle;
  RLIPanelInfo menu;
  RLIPanelInfo magn;
  QMap<QString, RLIPanelInfo> panels;
};


#endif // RLILAYOUT_H
