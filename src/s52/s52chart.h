#ifndef S52CHART_H
#define S52CHART_H

#include "ogr_core.h"

#include <QtOpenGL>
#include <QMap>
#include <QRectF>
#include <QString>

#include <vector>
#include "s52references.h"


class OGRLayer;
class OGRPoint;
class OGRFeature;
class OGRPolygon;
class OGRLineString;



struct S52AreaLayer {
  bool is_pattern_uniform;
  bool is_color_uniform;

  QString pattern_ref;
  uint color_ind;
  // layer i-th area s52 pattern name
  std::vector<QString> pattern_refs;
  // layer i-th area s52 color token
  //std::vector<QString> color_refs;
  std::vector<float> color_inds;
  // layer i-th area triangles start index
  std::vector<size_t> start_inds;
  // sequence of coords representing triangulated polygon
  std::vector<float> triangles;
};

struct S52LineLayer {
  bool is_pattern_uniform;
  bool is_color_uniform;

  QString pattern_ref;
  uint color_ind;

  // layer i-th line s52 pattern name
  std::vector<QString> pattern_refs;
  // layer i-th line s52 color token
  //std::vector<QString> color_refs;
  std::vector<float> color_inds;
  // layer i-th line points start index
  std::vector<size_t> start_inds;
  // sequence of coords representing polylines
  std::vector<float> points;
  // length of the line up to current point
  std::vector<double> distances;
};

struct S52MarkLayer {
  bool is_uniform;
  QString symbol_ref;
  // layer i-th point s52 symbol name
  std::vector<QString> symbol_refs;
  // sequence of point coords (lat, lon)
  std::vector<float> points;
};

struct S52TextLayer {
  // text marks
  std::vector<QString> texts;
  // sequence of point coords (lat, lon)
  std::vector<float> points;
};

// Sounding values
struct S52SndgLayer {
  // Depths
  std::vector<double> depths;
  // Sounding value point coords (lat, lon)
  std::vector<float> points;
};


class S52Chart {
public:
  S52Chart(char* file_name, S52References* ref);
  ~S52Chart();

  inline const QList<QString> areaLayerNames() const { return area_layers.keys(); }
  inline const QList<QString> lineLayerNames() const { return line_layers.keys(); }
  inline const QList<QString> markLayerNames() const { return mark_layers.keys(); }
  inline const QList<QString> textLayerNames() const { return text_layers.keys(); }

  inline S52AreaLayer* areaLayer(QString name) const { return area_layers.value(name, nullptr); }
  inline S52LineLayer* lineLayer(QString name) const { return line_layers.value(name, nullptr); }
  inline S52MarkLayer* markLayer(QString name) const { return mark_layers.value(name, nullptr); }
  inline S52TextLayer* textLayer(QString name) const { return text_layers.value(name, nullptr); }
  inline S52SndgLayer* sndgLayer()             const { return sndg_layer; }

  inline float minLat() const { return min_lat; }
  inline float maxLat() const { return max_lat; }
  inline float minLon() const { return min_lon; }
  inline float maxLon() const { return max_lon; }

private:
  bool isOk;
  S52References* _ref;
  void clear();

  // Chart dimension
  float min_lat, max_lat;
  float min_lon, max_lon;

  // Layered presentation of the chart
  QMap<QString, S52AreaLayer*> area_layers;
  QMap<QString, S52LineLayer*> line_layers;
  QMap<QString, S52MarkLayer*> mark_layers;
  QMap<QString, S52TextLayer*> text_layers;
  S52SndgLayer* sndg_layer;

  // Reads OGRLayer, appends presented layers to one or more layer maps
  bool readLayer(OGRLayer* poLayer, S52References* ref);
  bool readSoundingLayer(OGRLayer* poLayer, const QRectF& filterRect);
  bool readTextLayer(OGRLayer* poLayer);

  QMap<QString, QVariant> getOGRFeatureAttributes(OGRFeature* obj, const QMap<QString, std::pair<int, OGRFieldType>>& fields);

  // Reading and tesselating OGRPolygon, append result to triangles
  bool readOGRPolygon(OGRPolygon* poGeom, std::vector<float>& triangles);
  // Reading OGRLine, append result to points
  bool readOGRLine(OGRLineString* poGeom, std::vector<float>& points, std::vector<double>& distances);

  void fillLineParams(QString& layer_name, S52LineLayer* layer, OGRFeature* poFeature);
  void fillAreaParams(QString& layer_name, S52AreaLayer* layer, OGRFeature* poFeature);
  void fillMarkParams(S52MarkLayer* layer, LookUp* lp);

  QString getAreaColorRef(QString& layer_name, OGRFeature* poFeature);
  QString getAreaPatternRef(QString& layer_name, OGRFeature* poFeature);

  QString getLineColorRef(QString& layer_name, OGRFeature* poFeature);
  QString getLinePatternRef(QString& layer_name, OGRFeature* poFeature);

  QString getMarkSymbolRef(LookUp* lp);

  bool isAreaColorUniform(QString& layer_name);
  bool isAreaPatternUniform(QString& layer_name);

  bool isLineColorUniform(QString& layer_name);
  bool isLinePatternUniform(QString& layer_name);

  bool isMarkSymbolUniform(QString& layer_name);
};

#endif // S52CHART_H
