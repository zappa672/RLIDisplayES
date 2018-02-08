#ifndef S52REFERENCES_H
#define S52REFERENCES_H

#include <vector>

#include <QObject>
#include <QMap>
#include <QList>
#include <QColor>
#include <QVector2D>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QStringList>

class QXmlStreamReader;

struct ColorTable {
  QString name;
  QString graphics_file;
  QMap<QString, QColor> colors;
};

struct LookUp {
  int id;
  int rcid;

  QString name;
  QString type;
  QString disp_prio;
  QString radar_prio;
  QString table_name;
  QString instruction;
  QString display_cat;
  QStringList attr_refs;
  QString comment;
};

struct VectorSymbol {
  QVector2D size;
  QVector2D distance;
  QVector2D pivot;
  QVector2D origin;
  QString hpgl;
};

struct LineStyle {
  int rcid;
  QString name;

  VectorSymbol vector;
  QString description;
  QString color_ref;
};

struct Pattern {
  int rcid;
  QString name;

  QString definition;
  QString filltype;
  QString spacing;

  VectorSymbol vector;
  QString description;
  QString color_ref;
};

struct BitmapSymbol {
  QSize size;
  QVector2D distance;
  QPoint pivot;
  QPoint origin;
  QPoint graphics_location;
};

struct Symbol {
  int rcid;
  QString name;
  QString description;
  QString definition;
  QString color_ref;

  VectorSymbol vector;
  BitmapSymbol bitmap;
};


class S52References {
public:
  S52References(QString filename);
  ~S52References(void);

  QString getGraphicsFileName(const QString& scheme);

  QStringList getColorSchemeNames();

  QString getColorScheme();
  void setColorScheme(const QString& name);

  const QColor getColor(const QString& color_ref);

  int getColorIndex(const QString& color_ref);
  int getColorsCount() { return color_indices.size(); }
  ColorTable* getColorTable(const QString& color_scheme) { return color_tables[color_scheme]; }
  std::vector<float> getColorTable();

  const QPoint getSymbolIndex(const QString& symbol_ref);
  const QSize getSymbolDim(const QString& symbol_ref);
  const QPoint getSymbolPivot(const QString& symbol_ref);

private:
  void print(void);
  void fillColorTables2();

  QString color_scheme;

  void readColorTables(QXmlStreamReader* xml);
  void readLookUps(QXmlStreamReader* xml);
  void readLineStyles(QXmlStreamReader* xml);
  void readPatterns(QXmlStreamReader* xml);
  void readSymbols(QXmlStreamReader* xml);

  QMap<QString, int> color_indices;
  QMap<QString, std::vector<float> > color_tables2;

  QMap<QString, ColorTable*> color_tables;
  QMap<int, LookUp*> lookups;
  QMap<int, LineStyle*> line_styles;
  QMap<int, Pattern*> patterns;
  QMap<QString, Symbol*> symbols;
};

#endif // S52REFERENCES_H
