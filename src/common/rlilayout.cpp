#include "rlilayout.h"

#include <QFile>
#include <QDebug>
#include <QXmlStreamReader>
#include <QStringList>
#include <QRegExp>

const QRegExp SIZE_RE = QRegExp("^\\d{3,4}x\\d{3,4}$");

RLILayoutManager:: RLILayoutManager(const QString& filename) {
  QFile file(filename);
  file.open(QFile::ReadOnly);

  QXmlStreamReader* xml = new QXmlStreamReader(&file);

  while (!xml->atEnd()) {
    if (xml->readNext() == QXmlStreamReader::StartElement) {
      QMap<QString, QString> attrs = readXMLAttributes(xml);

      if (xml->name() == "layouts") {
        _defaultSize = attrs["default"];
        _currentSize = _defaultSize;
      }

      if (xml->name() == "layout") {
        QString size_tag = attrs["size"];
        _layouts.insert(size_tag, readLayout(parseSize(size_tag), xml));
      }
    }
  }

  file.close();
}

RLILayoutManager::~RLILayoutManager() {
}

QSize RLILayoutManager::currentSize() {
  return parseSize(_currentSize);
}

void RLILayoutManager::resize(const QSize& size) {
  int maxArea = 0;
  QString best = _defaultSize;

  for (auto size_tag : _layouts.keys()) {
    QSize sz = parseSize(size_tag);

    if (sz.width() <= size.width() && sz.height() <= size.height()) {
      int area(sz.width() * sz.height());

      if (area > maxArea) {
        best = size_tag;
        maxArea = area;
      }
    }
  }

  _currentSize = best;
}

QColor RLILayoutManager::parseColor(const QString& txt) const {
  QStringList slsize = txt.split(",");
  return QColor(slsize[0].toInt(), slsize[1].toInt(), slsize[2].toInt(), slsize[3].toInt());
}

QSize RLILayoutManager::parseSize(const QString& text) const {
  QStringList slsize = text.split("x");
  return QSize(slsize[0].toInt(), slsize[1].toInt());
}

QPoint RLILayoutManager::parsePoint(const QSize& scrn_sz, const QSize& sz, const QString& text) const {
  QStringList slpoint = text.split(",");
  int x = slpoint[0].replace("[^0-9]", "").toInt();
  int y = slpoint[1].replace("[^0-9]", "").toInt();

  if (x < 0) x += scrn_sz.width()  - sz.width();
  if (y < 0) y += scrn_sz.height() - sz.height();

  return QPoint(x, y);
}

QRect RLILayoutManager::parseRect(const QString& text) const {
  QStringList slsize = text.split("x");
  return QRect( QPoint(slsize[0].toInt(), slsize[1].toInt())
              , QSize(slsize[2].toInt(), slsize[3].toInt())  );
}

RLITextAllign RLILayoutManager::parseAllign(const QString& txt) const {
  if (txt == "left")
    return RLI_ALLIGN_LEFT;

  if (txt == "right")
    return RLI_ALLIGN_RIGHT;

  return RLI_ALLIGN_CENTER;
}


QMap<QString, QString> RLILayoutManager::readXMLAttributes(QXmlStreamReader* xml) {
  QMap<QString, QString> attributes;

  for (QXmlStreamAttribute attr : xml->attributes())
    attributes.insert(attr.name().toString(), attr.value().toString());

  return attributes;
}


RLILayout RLILayoutManager::readLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  RLILayout layout;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {

    case QXmlStreamReader::StartElement:
      if (xml->name() == "circle")
        layout.circle = readCircleLayout(scrn_sz, xml);

      if (xml->name() == "menu")
        layout.menu = readMenuLayout(scrn_sz, xml);

      if (xml->name() == "magnifier")
        layout.magnifier = readMagnifierLayout(scrn_sz, xml);

      if (xml->name() == "panel")
        layout.insertPanel(readInfoPanelLayout(scrn_sz, xml));
      break;

    case QXmlStreamReader::EndElement:
      if (xml->name() == "layout")
        return layout;
      break;

    default:
      break;
    }
  }

  return layout;
}


RLICircleLayout RLILayoutManager::readCircleLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  RLICircleLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.center = parsePoint(scrn_sz, scrn_sz, attrs["center"]);
  layout.radius = attrs["radius"].toInt();
  layout.font = attrs["font"];

  QPoint topLeft = layout.center - QPoint(layout.radius, layout.radius);
  QSize rectSize(2*layout.radius-1, 2*layout.radius-1);

  layout.bounding_rect = QRect(topLeft, rectSize);

  return layout;
}

RLIMenuLayout RLILayoutManager::readMenuLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  RLIMenuLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

  layout.geometry = QRect(pos, size);
  layout.font = attrs["font"];

  return layout;
}

RLIMagnifierLayout RLILayoutManager::readMagnifierLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  RLIMagnifierLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

  layout.geometry = QRect(pos, size);

  return layout;
}


RLIInfoPanelLayout RLILayoutManager::readInfoPanelLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  RLIInfoPanelLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

  layout.name = attrs["name"];
  layout.geometry = QRect(pos, size);
  layout.border_width = attrs["border_width"].toInt();
  layout.border_color = parseColor(attrs["border_color"]);
  layout.back_color = parseColor(attrs["back_color"]);
  layout.text_count = attrs["text_count"].toInt();

  while (!xml->atEnd()) {
    switch (xml->readNext()) {

    case QXmlStreamReader::StartElement:
      if (xml->name() == "text")
        layout.insertText(readInfoTextLayout(xml));

      if (xml->name() == "rect")
        layout.insertRect(readInfoRectLayout(xml));

      if (xml->name() == "table")
        layout.insertTable(readInfoTableLayout(xml));
      break;

    case QXmlStreamReader::EndElement:
      if (xml->name() == "panel")
        return layout;
      break;

    default:
      break;
    }
  }

  return layout;
}


RLIInfoTextLayout RLILayoutManager::readInfoTextLayout(QXmlStreamReader* xml) {
  RLIInfoTextLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.name = attrs["name"];
  layout.bounding_rect = parseRect(attrs["rect"]);
  layout.allign = parseAllign(attrs["allign"]);
  layout.font_tag = attrs["font"];
  layout.color = parseColor(attrs["color"]);

  return layout;
}

RLIInfoRectLayout RLILayoutManager::readInfoRectLayout(QXmlStreamReader* xml) {
  RLIInfoRectLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.name = attrs["name"];
  layout.rect = parseRect(attrs["rect"]);
  layout.color = parseColor(attrs["color"]);

  return layout;
}

RLIInfoTableLayout RLILayoutManager::readInfoTableLayout(QXmlStreamReader* xml) {
  RLIInfoTableLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.name = attrs["name"];
  layout.top = attrs["top"].toInt();
  layout.row_height = attrs["row_height"].toInt();
  layout.row_count = attrs["row_count"].toInt();

  while (!xml->atEnd()) {
    switch (xml->readNext()) {

    case QXmlStreamReader::StartElement:
      if (xml->name() == "column")
        layout.insertColumn(readInfoTableColumnLayout(xml));
      break;

    case QXmlStreamReader::EndElement:
      if (xml->name() == "table")
        return layout;
      break;

    default:
      break;
    }
  }

  return layout;
}

RLIInfoTableColumnLayout RLILayoutManager::readInfoTableColumnLayout(QXmlStreamReader* xml) {
  RLIInfoTableColumnLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.left = attrs["left"].toInt();
  layout.width = attrs["width"].toInt();
  layout.allign = parseAllign(attrs["allign"]);
  layout.font_tag = attrs["font"];
  layout.color = parseColor(attrs["color"]);

  return layout;
}
