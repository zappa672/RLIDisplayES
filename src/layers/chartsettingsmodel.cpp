#include "chartsettingsmodel.h"

#include <QXmlStreamReader>

ChartSettingsModel::ChartSettingsModel(const QString& file_path, QObject *parent) : QAbstractTableModel(parent), file(file_path) {
  load();
}

void ChartSettingsModel::load(void) {
  file.open(QFile::ReadOnly);
  layer_order.clear();
  layers_settings.clear();

  QXmlStreamReader* xml = new QXmlStreamReader(&file);
  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "DisplaySoundings")
        display_soundings = (xml->readElementText().trimmed().toLower() == "true");

      if (xml->name() == "Depths")
        readDepths(xml);

      if (xml->name() == "Layers")
        readLayers(xml);

      break;

    default:
      break;
    }
  }

  QList<LayerSettings> settings = layers_settings.values();
  qSort(settings);

  for (int i = 0; i < settings.size(); i++)
    layer_order.push_back(settings[i].name);

  file.close();
}

void ChartSettingsModel::readDepths(QXmlStreamReader* xml) {
  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "Shallow")
        shallow_depth = xml->readElementText().toFloat();

      if (xml->name() == "Safety")
        safety_depth = xml->readElementText().toFloat();

      if (xml->name() == "Deep")
        deep_depth = xml->readElementText().toFloat();

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "Depths")
        return;
      break;
    default:
      break;
    }
  }
}

void ChartSettingsModel::readLayers(QXmlStreamReader* xml) {
  LayerSettings ls;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "Id")
        ls.id = xml->readElementText().toInt();

      if (xml->name() == "Name")
        ls.name = xml->readElementText();

      if (xml->name() == "Descrption")
        ls.desc = xml->readElementText();

      if (xml->name() == "Display")
        ls.visible = (xml->readElementText().trimmed().toLower() == "true");

      if (xml->name() == "Order")
        ls.order = xml->readElementText().toInt();

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "Layer")
        layers_settings.insert(ls.name, ls);

      if (xml->name() == "Layers")
        return;

      break;
    default:
      break;
    }
  }
}

ChartSettingsModel::~ChartSettingsModel(void) {
  save();
}

void ChartSettingsModel::save(void) {
  file.open(QFile::WriteOnly);

  QXmlStreamWriter* xml = new QXmlStreamWriter(&file);
  xml->setAutoFormatting(true);
  xml->writeStartDocument();

  xml->writeStartElement("Settings");

  xml->writeTextElement("DisplaySoundings" , display_soundings ? "True" : "False");

  xml->writeStartElement("Depths");
  xml->writeTextElement("Shallow" , QString::number(shallow_depth));
  xml->writeTextElement("Safety"  , QString::number(safety_depth));
  xml->writeTextElement("Deep"    , QString::number(deep_depth));
  xml->writeEndElement();

  xml->writeStartElement("Layers");

  int order = 1;
  for (int i = 0; i < layer_order.size(); i++) {
    LayerSettings ls = layers_settings[layer_order[i]];
    xml->writeStartElement("Layer");
    xml->writeTextElement("Id"        , QString::number(ls.id));
    xml->writeTextElement("Name"      , ls.name);
    xml->writeTextElement("Descrption", ls.desc);
    xml->writeTextElement("Display"   , ls.visible ? "True" : "False");
    xml->writeTextElement("Order"   , QString::number(order++));
    xml->writeEndElement();
  }
  xml->writeEndElement();

  xml->writeEndElement();

  file.close();
}

QStringList ChartSettingsModel::getLayersDisplayOrder() {
  return layer_order;
}

void ChartSettingsModel::swapOrders(int i, int j) {
  emit layoutAboutToBeChanged();

  if (i >= 0 && j >= 0 && i < layer_order.size() && j < layer_order.size()) {
    QString temp = layer_order[i];
    layer_order[i] = layer_order[j];
    layer_order[j] = temp;
  }

  emit layoutChanged();
}

bool ChartSettingsModel::isLayerVisible(const QString& layer_name) {
  if (layers_settings.contains(layer_name))
    return layers_settings[layer_name].visible;

  return false;
}

void ChartSettingsModel::setLayerVisibility(const QString& layer_name, bool val) {
  layers_settings[layer_name] = LayerSettings(layer_name);
  layers_settings[layer_name].visible = val;
}

float ChartSettingsModel::getShallowDepth(void) {
  return shallow_depth;
}

float ChartSettingsModel::getSafetyDepth(void) {
  return safety_depth;
}

float ChartSettingsModel::getDeepDepth(void) {
  return deep_depth;
}

void ChartSettingsModel::setShallowDepth(float val) {
  shallow_depth = val;
}

void ChartSettingsModel::setSafetyDepth(float val) {
  safety_depth = val;
}

void ChartSettingsModel::setDeepDepth(float val) {
  deep_depth = val;
}

bool ChartSettingsModel::areSoundingsVisible() {
  return display_soundings;
}

void ChartSettingsModel::setSoundingsVisible(bool val) {
  display_soundings = val;
}

int ChartSettingsModel::rowCount(const QModelIndex& /*parent*/) const {
  return layer_order.size();
}

int ChartSettingsModel::columnCount(const QModelIndex& /*parent*/) const {
  return 3;
}

QVariant ChartSettingsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.column() == 0 && role == Qt::DisplayRole)
    return layer_order[index.row()];

  if (index.column() == 1 && role == Qt::CheckStateRole)
    return layers_settings[layer_order[index.row()]].visible ? Qt::Checked : Qt::Unchecked;

  if (index.column() == 2 && role == Qt::DisplayRole)
    return layers_settings[layer_order[index.row()]].desc;

  return QVariant();
}

bool ChartSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid() || index.column() != 1)
    return false;

  //qDebug() << role;

  if (role == Qt::CheckStateRole) {
    layers_settings[layer_order[index.row()]].visible = ((Qt::CheckState)value.toInt() == Qt::Checked);
    return true;
  }

  return false;
}

Qt::ItemFlags ChartSettingsModel::flags(const QModelIndex &index) const {
  switch (index.column()) {
    case 0:
      return Qt::NoItemFlags;
      break;
    case 1:
      return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
      break;
    case 2:
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
      break;
    default:
      return Qt::NoItemFlags;
      break;
  }
}

QVariant ChartSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  switch (orientation) {
    case Qt::Vertical:
      return QVariant(section + 1);
      break;
    case Qt::Horizontal:
      switch (section) {
        case 0:
          return QVariant("Name");
          break;
        case 1:
          return QVariant("Visible");
          break;
        case 2:
          return QVariant("Description");
          break;
        default:
          return QVariant();
          break;
      }
      break;
    default:
      return QVariant();
      break;
  }
}
