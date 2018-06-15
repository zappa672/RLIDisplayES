#include "rliconfig.h"

#include <QFile>
#include <QDebug>
#include <QXmlStreamReader>
#include <QStringList>
#include <QRegExp>

const QRegExp SIZE_RE = QRegExp("^\\d{3,4}x\\d{3,4}$");

RLIConfig::RLIConfig(const QString& filename) {
  QFile file(filename);
  file.open(QFile::ReadOnly);

  QXmlStreamReader* xml = new QXmlStreamReader(&file);

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "layouts") {
        QMap<QString, QString> attrs = readXMLAttributes(xml);
        _defaultSize = attrs["default"];
        _currentSize = _defaultSize;
      }

      if (xml->name() == "layout") {
        QMap<QString, QString> attrs = readXMLAttributes(xml);
        QSize sz = parseSize(attrs["size"]);

        RLILayout* layout = readLayout(sz, xml);

        _layouts.insert(attrs["size"], layout);
      }
      break;
    default:
      break;
    }
  }

  file.close();
}

RLIConfig::~RLIConfig() {
  qDeleteAll(_layouts);
}

QSize RLIConfig::parseSize(const QString& text) {
  QStringList slsize = text.split("x");
  return QSize(slsize[0].toInt(), slsize[1].toInt());
}

QPoint RLIConfig::parsePoint(const QSize& scr_sz, const QSize& sz, const QString& text) {
  QStringList slpoint = text.split(",");
  int x = slpoint[0].replace("[^0-9]", "").toInt();
  int y = slpoint[1].replace("[^0-9]", "").toInt();

  if (x < 0) x += scr_sz.width()  - sz.width();
  if (y < 0) y += scr_sz.height() - sz.height();

  return QPoint(x, y);
}

RLILayout* RLIConfig::readLayout(const QSize& scr_sz, QXmlStreamReader* xml) {
  RLILayout* layout = new RLILayout;
  QMap<QString, QString> params;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "circle") {
        params = readXMLAttributes(xml);
        RLICircleInfo cInfo;

        cInfo.radius = params["radius"].toInt();
        QSize rectSize(2*cInfo.radius-1, 2*cInfo.radius-1);

        cInfo.center = parsePoint(scr_sz, rectSize, params["center"]);
        QPoint topLeft = cInfo.center - QPoint(cInfo.radius, cInfo.radius);

        cInfo.boundRect = QRect(topLeft, rectSize);
        cInfo.font = params["font"];

        layout->circle = cInfo;
      }

      if (xml->name() == "menu") {
        parsePanelAttributes(scr_sz, xml, &layout->menu);
      }

      if (xml->name() == "magnifier") {
        parsePanelAttributes(scr_sz, xml, &layout->magn);
      }

      if (xml->name() == "panels") {
        layout->panels = readPanelLayouts(scr_sz, xml);
      }

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

RLIPanelTableInfo RLIConfig::readTableInfo(QXmlStreamReader* xml) {
  RLIPanelTableInfo tableInfo;
  tableInfo.params = readXMLAttributes(xml);

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "column")
        tableInfo.columns.push_back(readXMLAttributes(xml));

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "table")
        return tableInfo;

      break;
    default:
      break;
    }
  }

  return tableInfo;
}

void RLIConfig::parsePanelAttributes(const QSize& scr_sz, QXmlStreamReader* xml, RLIPanelInfo* panel) {
  panel->params = readXMLAttributes(xml);
  panel->size = parseSize(panel->params["size"]);
  panel->position = parsePoint(scr_sz, panel->size, panel->params["pos"]);
}

QMap<QString, RLIPanelInfo> RLIConfig::readPanelLayouts(const QSize& scr_sz, QXmlStreamReader* xml) {
  QMap<QString, RLIPanelInfo> panels;
  RLIPanelInfo current_panel;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "panel" || xml->name() == "value-bar" || xml->name() == "label") {
        current_panel.clear();
        parsePanelAttributes(scr_sz, xml, &current_panel);
      }

      if (xml->name() == "text") {
        auto attrs = readXMLAttributes(xml);
        current_panel.texts.insert(attrs["name"], attrs);
      }

      if (xml->name() == "rect") {
        auto attrs = readXMLAttributes(xml);
        current_panel.rects.insert(attrs["name"], attrs);
      }

      if (xml->name() == "table") {
        auto tblInfo = readTableInfo(xml);
        current_panel.tables.insert(tblInfo.params["name"], tblInfo);
      }

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "panel" || xml->name() == "value-bar" || xml->name() == "label")
        panels.insert(current_panel.params["name"], current_panel);

      if (xml->name() == "panels")
        return panels;

      break;
    default:
      break;
    }
  }

  return panels;
}

QMap<QString, QString> RLIConfig::readXMLAttributes(QXmlStreamReader* xml) {
  QMap<QString, QString> attributes;

  for (QXmlStreamAttribute attr : xml->attributes())
    attributes.insert(attr.name().toString(), attr.value().toString());

  return attributes;
}


const QSize RLIConfig::currentSize() const {
  QStringList slsize = _currentSize.split("x");
  return QSize(slsize[0].toInt(), slsize[1].toInt());
}


void RLIConfig::setCurrentSize(const QSize& screen_size) {
  QString best = "";
  int max_area = 0;

  for (QString ssize : _layouts.keys()) {
    QStringList slsize = ssize.split("x");
    QSize size(slsize[0].toInt(), slsize[1].toInt());

    if (size.width() <= screen_size.width() && size.height() <= screen_size.height()) {
      float area = size.width() * size.height();

      if (area > max_area) {
        max_area = area;
        best = ssize;
      }
    }
  }

  if (best == "")
    _currentSize = _defaultSize;
  else
    _currentSize = best;
}
