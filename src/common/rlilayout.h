#ifndef RLILAYOUT_H
#define RLILAYOUT_H

#include <QString>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QColor>
#include <QVector>
#include <QMap>

class QXmlStreamReader;


typedef enum RLITextAllign {
  RLI_ALLIGN_LEFT
, RLI_ALLIGN_RIGHT
, RLI_ALLIGN_CENTER
} RLITextAllign;


struct RLIInfoRectLayout {
  QString name;
  QRect   rect;
  QColor  color;
};

struct RLIInfoTextLayout {
  QString name;
  QRect   bounding_rect;
  RLITextAllign allign;
  QString font_tag;
};

struct RLIInfoTableColumnLayout {
  int left, width;
  RLITextAllign allign;
  QString font_tag;
};

struct RLIInfoTableLayout {
  QString name;
  int top, row_height;
  QVector<RLIInfoTableColumnLayout> columns;

  inline void insertColumn(const RLIInfoTableColumnLayout& layout) { columns.push_back(layout); }
};

struct RLILabelLayout {
  QString name;
  QRect geometry;
  QString font_tag;
};

struct RLIValueBarLayout {
  QString name;
  QRect geometry;
  QString font_tag;
  int bar_width;
};

struct RLIInfoPanelLayout {
  QString name;
  QRect geometry;

  QMap<QString, RLIInfoTextLayout> texts;
  QMap<QString, RLIInfoRectLayout> rects;
  QMap<QString, RLIInfoTableLayout> tables;

  inline void insertRect(const RLIInfoRectLayout& layout) { rects.insert(layout.name, layout); }
  inline void insertText(const RLIInfoTextLayout& layout) { texts.insert(layout.name, layout); }
  inline void insertTable(const RLIInfoTableLayout& layout) { tables.insert(layout.name, layout); }
};


struct RLICircleLayout {
  QPoint center;
  int radius;
  QString font;
  QRect bounding_rect;
};

struct RLIMenuLayout {
  QRect geometry;
  QString font;
};

struct RLIMagnifierLayout {
  QRect geometry;
};


struct RLILayout {
  RLICircleLayout     circle;
  RLIMenuLayout       menu;
  RLIMagnifierLayout  magnifier;

  QMap<QString, RLILabelLayout> labels;
  QMap<QString, RLIValueBarLayout> value_bars;
  QMap<QString, RLIInfoPanelLayout> panels;

  inline void insertLabel(const RLILabelLayout& layout) { labels.insert(layout.name, layout); }
  inline void insertValueBar(const RLIValueBarLayout& layout) { value_bars.insert(layout.name, layout); }
  inline void insertPanel(const RLIInfoPanelLayout& layout) { panels.insert(layout.name, layout); }
};



class  RLILayoutManager {
public:
   RLILayoutManager(const QString& filename);
  ~RLILayoutManager(void);

  QSize currentSize();
  void resize(const QSize& size);

  RLICircleLayout circleLayout();
  RLIMenuLayout menuLayout();
  RLIMagnifierLayout magnifierLayout();

  RLILabelLayout labelLayout(const QString& name);
  RLIValueBarLayout valueBarLayout(const QString& name);
  RLIInfoPanelLayout panelLayout(const QString& name);

private:
  QMap<QString, RLILayout> _layouts;
  QString _currentSize, _defaultSize;

  QMap<QString, QString> readXMLAttributes(QXmlStreamReader* xml);

  QSize               parseSize           (const QString& txt) const;
  QPoint              parsePoint          (const QSize& scrn_sz, const QSize& sz, const QString& txt) const;
  QRect               parseRect           (const QString& txt) const;
  RLITextAllign       parseAllign         (const QString& txt) const;

  RLILayout           readLayout          (const QSize& scrn_sz, QXmlStreamReader* xml);

  RLICircleLayout     readCircleLayout    (const QSize& scrn_sz, QXmlStreamReader* xml);
  RLIMenuLayout       readMenuLayout      (const QSize& scrn_sz, QXmlStreamReader* xml);
  RLIMagnifierLayout  readMagnifierLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

  RLILabelLayout      readLabelLayout     (const QSize& scrn_sz, QXmlStreamReader* xml);
  RLIValueBarLayout   readValueBarLayout  (const QSize& scrn_sz, QXmlStreamReader* xml);
  RLIInfoPanelLayout  readInfoPanelLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

  RLIInfoTextLayout         readInfoTextLayout        (QXmlStreamReader* xml);
  RLIInfoRectLayout         readInfoRectLayout        (QXmlStreamReader* xml);
  RLIInfoTableLayout        readInfoTableLayout       (QXmlStreamReader* xml);
  RLIInfoTableColumnLayout  readInfoTableColumnLayout (QXmlStreamReader* xml);
};

#endif // RLILAYOUT_H
