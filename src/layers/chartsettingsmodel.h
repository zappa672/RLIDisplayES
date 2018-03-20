#ifndef CHARTSETTINGSMODEL_H
#define CHARTSETTINGSMODEL_H

#include <QObject>
#include <QString>
#include <QAbstractTableModel>
#include <QFile>
#include <QMap>
#include <QXmlStreamReader>

struct ChartLayerDisplaySettings {
  int id;
  QString name;
  QString desc;
  bool visible;
  int order;

  ChartLayerDisplaySettings() {
    id = -1;
    name = "";
    desc = "";
    visible = false;
    order = 1000;
  }

  ChartLayerDisplaySettings(const QString& layer_name) {
    id = -1;
    name = layer_name;
    desc = layer_name;
    visible = false;
    order = 1000;
  }

  bool operator<(const ChartLayerDisplaySettings& rhs) const {
    return order < rhs.order;
  }
};

class ChartSettingsModel : public QAbstractTableModel {
    Q_OBJECT
public:
  ChartSettingsModel(const QString& file_path, QObject *parent = 0);
  ~ChartSettingsModel(void);

  int rowCount(const QModelIndex &parent = QModelIndex()) const ;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);

  void load(void);
  void save(void);

  QStringList getLayersDisplayOrder();
  void swapOrders(int i, int j);

  bool isLayerVisible(const QString& layer_name);
  void setLayerVisibility(const QString& layer_name, bool val);

  bool areSoundingsVisible();
  void setSoundingsVisible(bool val);

  float getShallowDepth(void);
  void setShallowDepth(float val);
  float getSafetyDepth(void);
  void setSafetyDepth(float val);
  float getDeepDepth(void);
  void setDeepDepth(float val);

  inline ChartLayerDisplaySettings layerSettings(QString name) { return layers_settings.value(name, ChartLayerDisplaySettings()); }

private:
  void readDepths(QXmlStreamReader* xml);
  void readLayers(QXmlStreamReader* xml);

  QFile file;

  bool display_soundings;
  QMap<QString, ChartLayerDisplaySettings> layers_settings;
  QStringList layer_order;

  float shallow_depth;
  float safety_depth;
  float deep_depth;
};

#endif // CHARTSETTINGSMODEL_H
