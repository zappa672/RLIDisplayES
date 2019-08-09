#ifndef S52CHART_H
#define S52CHART_H

#include "ogrsf_frmts.h"

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
  std::vector<QString>  pattern_refs;   // layer i-th area s52 pattern name
  std::vector<float>    color_inds;     // layer i-th area s52 color token
  std::vector<float>    disp_prio;
  std::vector<size_t>   start_inds;     // layer i-th area triangles start index
  std::vector<float>    triangles;      // sequence of coords representing triangulated polygon

};

struct S52LineLayer {
  std::vector<QString>  pattern_refs;   // layer i-th line s52 pattern name
  std::vector<float>    color_inds;     // layer i-th line s52 color token
  std::vector<float>    disp_prio;
  std::vector<size_t>   start_inds;     // layer i-th line points start index
  std::vector<float>    points;         // sequence of coords representing polylines
  std::vector<double>   distances;      // length of the line up to current point
};

struct S52MarkLayer {  
  std::vector<QString>  symbol_refs;    // layer i-th point s52 symbol name
  std::vector<float>    points;         // sequence of point coords (lat, lon)
  std::vector<float>    disp_prio;
};

struct S52TextLayer {
  std::vector<QString>  texts;          // text marks
  std::vector<float>    points;         // sequence of point coords (lat, lon)
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

  QSet<int> floatingATONArray;
  QSet<int> rigidATONArray;
  double _m_next_safe_cnt = 1.0e6;

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
  bool readLayer(OGRLayer* poLayer, S52References* ref, OGRDataSource* ds);
  bool readSoundingLayer(OGRLayer* poLayer, const OGRGeometry* spatFilter);
  bool readTextLayer(OGRLayer* poLayer);

  QMap<QString, QVariant> getOGRFeatureAttributes(OGRFeature* obj, const QMap<QString, std::pair<int, OGRFieldType>>& fields);

  bool addAreaToLayer(S52AreaLayer* layer, const QString& ptrn_ref, const QString& col_ref, ChartDispPrio dpri, OGRPolygon* poly);
  // Reading and tesselating OGRPolygon, append result to triangles
  bool readOGRPolygon(OGRPolygon* poGeom, std::vector<float>& triangles);

  bool addLineToLayer(S52LineLayer* layer, const QString& ptrn_ref, const QString& col_ref, ChartDispPrio dpri, OGRLineString* line);
  // Reading OGRLine, append result to points
  bool readOGRLine(OGRLineString* poGeom, std::vector<float>& points, std::vector<double>& distances);
};

#endif // S52CHART_H
