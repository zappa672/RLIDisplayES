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




enum class LookUpTable {
    LUP_TABLE_SIMPLIFIED             = 'L' //points
  , LUP_TABLE_PAPER_CHART            = 'R' //points
  , LUP_TABLE_LINES                  = 'S' //lines
  , LUP_TABLE_PLAIN_BOUNDARIES       = 'N' //areas
  , LUP_TABLE_SYMBOLYZED_BOUNDARIES  = 'O' //areas
  , LUP_TABLE_COUNT                  = 5
};

static const QMap<QString, LookUpTable> LOOKUP_TYPE_MAP
{ { "Lines"     , LookUpTable::LUP_TABLE_LINES }
, { "Symbolized", LookUpTable::LUP_TABLE_SYMBOLYZED_BOUNDARIES }
, { "Plain"     , LookUpTable::LUP_TABLE_PLAIN_BOUNDARIES }
, { "Simplified", LookUpTable::LUP_TABLE_SIMPLIFIED }
, { "Paper"     , LookUpTable::LUP_TABLE_PAPER_CHART } };



// Addressed Object Type
enum class ChartObjectType {
  CHART_OBJ_TYPE_POINT  = 'P'
, CHART_OBJ_TYPE_LINES  = 'L'
, CHART_OBJ_TYPE_AREAS  = 'A'
, CHART_OBJ_TYPE_COUNT  = 3     // number of object type
};

static const QMap<QString, ChartObjectType> CHART_OBJ_TYPE_MAP
{ { "Line" , ChartObjectType::CHART_OBJ_TYPE_LINES }
, { "Point", ChartObjectType::CHART_OBJ_TYPE_POINT }
, { "Area" , ChartObjectType::CHART_OBJ_TYPE_AREAS } };



// Display Priority
enum class ChartDispPrio {
  DISP_PRIO_NODATA          = 0   // no data fill area pattern
, DISP_PRIO_GROUP1          = 1   // S57 group 1 filled areas
, DISP_PRIO_AREA_1          = 2   // superimposed areas
, DISP_PRIO_AREA_2          = 3   // superimposed areas also water features
, DISP_PRIO_SYMB_POINT      = 4   // point symbol also land features
, DISP_PRIO_SYMB_LINE       = 5   // line symbol also restricted areas
, DISP_PRIO_SYMB_AREA       = 6   // area symbol also traffic areas
, DISP_PRIO_ROUTING         = 7   // routeing lines
, DISP_PRIO_HAZARDS         = 8   // hazards
, DISP_PRIO_MARINERS        = 9   // VRM, EBL, own ship
, DISP_PRIO_NUM             = 10  // number of priority levels
};

static const QMap<QString, ChartDispPrio> CHART_DISP_PRIO_MAP
{ { "Hazards"     , ChartDispPrio::DISP_PRIO_HAZARDS }
, { "Group 1"     , ChartDispPrio::DISP_PRIO_GROUP1 }
, { "Line Symbol" , ChartDispPrio::DISP_PRIO_SYMB_LINE }
, { "Point Symbol", ChartDispPrio::DISP_PRIO_SYMB_POINT }
, { "Area 1"      , ChartDispPrio::DISP_PRIO_AREA_1 }
, { "Routing"     , ChartDispPrio::DISP_PRIO_ROUTING }
, { "Mariners"    , ChartDispPrio::DISP_PRIO_MARINERS }
, { "Area 2"      , ChartDispPrio::DISP_PRIO_AREA_2 }
, { "Area Symbol" , ChartDispPrio::DISP_PRIO_SYMB_AREA }
, { "No data"     , ChartDispPrio::DISP_PRIO_HAZARDS } };



// RADAR Priority
enum class ChartRadarPrio {
  RAD_PRIO_OVER = 'O'  // presentation on top of RADAR
, RAD_PRIO_SUPP = 'S'  // presentation suppressed by RADAR
, RAD_PRIO_NUM  = 2
};

static const QMap<QString, ChartRadarPrio> CHART_RADAR_PRIO_MAP
{ { "On Top"    , ChartRadarPrio::RAD_PRIO_OVER }
, { "Supressed" , ChartRadarPrio::RAD_PRIO_SUPP } };


// display category type
enum class ChartDisplayCat {
  DISP_CAT_DISPLAYBASE          = 'D'  //
, DISP_CAT_STANDARD             = 'S'  //
, DISP_CAT_OTHER                = 'O'  // O for OTHER
, DISP_CAT_MARINERS_STANDARD    = 'M'  // Mariner specified
, DISP_CAT_MARINERS_OTHER              // value not defined
};


static const QMap<QString, ChartDisplayCat> CHART_DISPLAY_CAT_MAP
{ { "Displaybase" , ChartDisplayCat::DISP_CAT_DISPLAYBASE }
, { "Standard"    , ChartDisplayCat::DISP_CAT_STANDARD }
, { "Other"       , ChartDisplayCat::DISP_CAT_OTHER }
, { "Mariners"    , ChartDisplayCat::DISP_CAT_MARINERS_STANDARD } };




#define MASK_POINT      1
#define MASK_LINE       2
#define MASK_AREA       4
#define MASK_MPS        8
#define MASK_ALL        MASK_POINT + MASK_LINE + MASK_AREA + MASK_MPS

// Rasterization rule types
enum class RastRuleType {
  RUL_NONE     // no rule type (init)
, RUL_TXT_TX   // TX
, RUL_TXT_TE   // TE
, RUL_SYM_PT   // SY
, RUL_SIM_LN   // LS
, RUL_COM_LN   // LC
, RUL_ARE_CO   // AC
, RUL_ARE_PA   // AP
, RUL_CND_SY   // CS
, RUL_MUL_SG   // Multipoint Sounding
, RUL_ARC_2C   // Circular Arc, used for sector lights, opencpn private
};

static const QMap<QString, RastRuleType> RAST_RULE_TYPE_MAP
{ { "CA" , RastRuleType::RUL_ARC_2C }
, { "MP" , RastRuleType::RUL_MUL_SG }
, { "TX" , RastRuleType::RUL_TXT_TX }
, { "TE" , RastRuleType::RUL_TXT_TE }
, { "SY" , RastRuleType::RUL_SYM_PT }
, { "LS" , RastRuleType::RUL_SIM_LN }
, { "LC" , RastRuleType::RUL_COM_LN }
, { "AC" , RastRuleType::RUL_ARE_CO }
, { "AP" , RastRuleType::RUL_ARE_PA }
, { "CS" , RastRuleType::RUL_CND_SY } };




struct ColorTable {
  QString name;
  QString graphics_file;
  QMap<QString, QColor> colors;
  std::vector<float> table;
};


struct LookUp {
  int        nSequence;   // A sequence number, indicating order of encounter in the PLIB file
  int             RCID = -1;   // record identifier

  QStringList     INST;   // Instruction Field (rules)
  QStringList     ALST;   // Array of LUP Attributes

  QString         OBCL;   // Name (6 char) '\0' terminated
  ChartObjectType FTYP;   // 'A' Area, 'L' Line, 'P' Point
  ChartDispPrio   DPRI;   // Display Priority
  ChartRadarPrio  RPRI;   // 'O' or 'S', Radar Priority
  LookUpTable     TNAM;   // FTYP:  areas, points, lines
  ChartDisplayCat DISC;   // Display Categorie: D/S/O, DisplayBase, Standard, Other
  int             LUCM;   // Look-Up Comment (PLib3.x put 'groupes' here,
                          // hence 'int', but its a string in the specs)
};

struct VectorSymbol {
  QSize size;
  QVector2D distance;
  QPoint pivot;
  QPoint origin;
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
  QSize     size      { 0, 0 };
  QVector2D distance  { 0, 0 };
  QPoint    pivot     { 0, 0 };
  QPoint    origin    { 0, 0 };
  QPoint    graphics_location { 0, 0 };
};

struct Symbol {
  int rcid            { 0 };
  QString name        {};
  QString description {};
  QString definition  {};
  QString color_ref   {};

  VectorSymbol vector {};
  BitmapSymbol bitmap {};
};


class S52References {
public:
  S52References(QString filename);
  ~S52References(void);

  LookUp findBestLookUp(const QString& name, const QMap<QString, QVariant>& objAttrs, LookUpTable tbl);

  inline QString getGraphicsFileName(const QString& scheme) const {return _colTbls[scheme]->graphics_file;}

  void setColorScheme(const QString& name);
  QColor getColor(const QString& color_ref) const { return _colTbls[_colorScheme]->colors.value(color_ref, QColor(255, 255, 255, 0)); }

  inline QStringList  getColorSchemeNames() const { return QStringList(_colTbls.keys()); }
  inline QString      getColorScheme     () const { return _colorScheme; }

  inline uint               getColorIndex    (const QString& ref)     const { return _colorIndices.value(ref, -1u); }
  inline int                getColorsCount   ()                       const { return _colorIndices.size(); }
  inline ColorTable*        getColorTable    (const QString& scheme)  const { return _colTbls[scheme]; }
  inline std::vector<float> getCurrColorTable()                       const { return _colTbls[_colorScheme]->table; }

  inline const QPoint  getSymbolIndex(const QString& ref) const { return symbols.value(ref, Symbol()).bitmap.graphics_location; }
  inline const QSize   getSymbolSize (const QString& ref) const { return symbols.value(ref, Symbol()).bitmap.size; }
  inline const QPoint  getSymbolPivot(const QString& ref) const { return symbols.value(ref, Symbol()).bitmap.pivot; }

private:
  void print(void);
  void fillColorTables();

  void readColorTables(QXmlStreamReader* xml);
  void readLookUps    (QXmlStreamReader* xml);
  void readLineStyles (QXmlStreamReader* xml);
  void readPatterns   (QXmlStreamReader* xml);
  void readSymbols    (QXmlStreamReader* xml);

  QString _colorScheme;

  QMap<QString, uint> _colorIndices;
  QMap<QString, ColorTable*> _colTbls;

  QMap<LookUpTable, QMap<QString, QVector<LookUp>>> lookups;

  QMap<int, LineStyle>  line_styles;
  QMap<int, Pattern>    patterns;

  QMap<QString, Symbol> symbols;
};

#endif // S52REFERENCES_H
