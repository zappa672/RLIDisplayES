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
  QRect bounding_rect;
  RLITextAllign allign;
  QString font_tag;
  QColor color;
};

struct RLIInfoTableColumnLayout {
  int left, width;
  RLITextAllign allign;
  QString font_tag;
  QColor color;
};

struct RLIInfoTableLayout {
  QString name;
  int row_count, top, row_height;
  QVector<RLIInfoTableColumnLayout> columns;

  inline void insertColumn(const RLIInfoTableColumnLayout& layout) { columns.push_back(layout); }
};

struct RLIInfoPanelLayout {
  QString name;
  QRect geometry;
  int border_width;
  QColor back_color;
  QColor border_color;
  int text_count;

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
  int mode_symb_shift;
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

  QMap<QString, RLIInfoPanelLayout> panels;
  inline void insertPanel(const RLIInfoPanelLayout& layout) { panels.insert(layout.name, layout); }
};



class  RLILayoutManager {
public:
   RLILayoutManager(const QString& filename);
  ~RLILayoutManager(void);

  QSize currentSize();
  void resize(const QSize& size);

  inline RLILayout* layout() { return &_layouts[_currentSize]; }

private:
  QMap<QString, RLILayout> _layouts;
  QString _currentSize, _defaultSize;

  QMap<QString, QString> readXMLAttributes(QXmlStreamReader* xml);

  QColor              parseColor          (const QString& txt) const;
  QSize               parseSize           (const QString& txt) const;
  QPoint              parsePoint          (const QSize& scrn_sz, const QSize& sz, const QString& txt) const;
  QRect               parseRect           (const QString& txt) const;
  RLITextAllign       parseAllign         (const QString& txt) const;

  RLILayout           readLayout          (const QSize& scrn_sz, QXmlStreamReader* xml);

  RLICircleLayout     readCircleLayout    (const QSize& scrn_sz, QXmlStreamReader* xml);
  RLIMenuLayout       readMenuLayout      (const QSize& scrn_sz, QXmlStreamReader* xml);
  RLIMagnifierLayout  readMagnifierLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

  RLIInfoPanelLayout  readInfoPanelLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

  RLIInfoTextLayout         readInfoTextLayout        (QXmlStreamReader* xml);
  RLIInfoRectLayout         readInfoRectLayout        (QXmlStreamReader* xml);
  RLIInfoTableLayout        readInfoTableLayout       (QXmlStreamReader* xml);
  RLIInfoTableColumnLayout  readInfoTableColumnLayout (QXmlStreamReader* xml);
};

#endif // RLILAYOUT_H
