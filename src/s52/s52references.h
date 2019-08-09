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

#include "../common/rlistate.h"

const static RLIDepthUnit m_nDepthUnitDisplay = RLIDepthUnit::METER; // feet/meters

const static bool S52_MAR_TWO_SHADES = true;

const static double CONST_SHALLOW_DEPTH = 5.0;
const static double CONST_SAFETY_DEPTH  = 10.0;
const static double CONST_DEEP_DEPTH    = 25.0;

enum class LookUpTable {
  SIMPLIFIED             = 'L' //points
, PAPER_CHART            = 'R' //points
, LINES                  = 'S' //lines
, PLAIN_BOUNDARIES       = 'N' //areas
, SYMBOLYZED_BOUNDARIES  = 'O' //areas
, COUNT                  = 5
};

const static LookUpTable CONST_SYMB_LOOKUP = LookUpTable::PAPER_CHART;      // Can be set to SIMPLIFIED
const static LookUpTable CONST_AREA_LOOKUP = LookUpTable::PLAIN_BOUNDARIES; // Can be set to SYMBOLYZED_BOUNDARIES


class QXmlStreamReader;



static const QMap<QString, LookUpTable> LOOKUP_TYPE_MAP
{ { "Lines"     , LookUpTable::LINES }
, { "Symbolized", LookUpTable::SYMBOLYZED_BOUNDARIES }
, { "Plain"     , LookUpTable::PLAIN_BOUNDARIES }
, { "Simplified", LookUpTable::SIMPLIFIED }
, { "Paper"     , LookUpTable::PAPER_CHART } };



// Addressed Object Type
enum class ChartObjectType {
  POINT  = 'P'
, LINE   = 'L'
, AREA   = 'A'
, COUNT  = 3     // number of object type
};

static const QMap<QString, ChartObjectType> CHART_OBJ_TYPE_MAP
{ { "Line" , ChartObjectType::LINE  }
, { "Point", ChartObjectType::POINT }
, { "Area" , ChartObjectType::AREA  } };



// Display Priority
enum class ChartDispPrio: int {
  NODATA          = 0   // no data fill area pattern
, GROUP1          = 1   // S57 group 1 filled areas
, AREA_1          = 2   // superimposed areas
, AREA_2          = 3   // superimposed areas also water features
, SYMB_POINT      = 4   // point symbol also land features
, SYMB_LINE       = 5   // line symbol also restricted areas
, SYMB_AREA       = 6   // area symbol also traffic areas
, ROUTING         = 7   // routeing lines
, HAZARDS         = 8   // hazards
, MARINERS        = 9   // VRM, EBL, own ship
, NUM             = 10  // number of priority levels
};

static const QMap<QString, ChartDispPrio> CHART_DISP_PRIO_MAP
{ { "Hazards"     , ChartDispPrio::HAZARDS }
, { "Group 1"     , ChartDispPrio::GROUP1 }
, { "Line Symbol" , ChartDispPrio::SYMB_LINE }
, { "Point Symbol", ChartDispPrio::SYMB_POINT }
, { "Area 1"      , ChartDispPrio::AREA_1 }
, { "Routing"     , ChartDispPrio::ROUTING }
, { "Mariners"    , ChartDispPrio::MARINERS }
, { "Area 2"      , ChartDispPrio::AREA_2 }
, { "Area Symbol" , ChartDispPrio::SYMB_AREA }
, { "No data"     , ChartDispPrio::HAZARDS } };



// RADAR Priority
enum class ChartRadarPrio {
  OVER = 'O'  // presentation on top of RADAR
, SUPP = 'S'  // presentation suppressed by RADAR
, NUM  = 2
};

static const QMap<QString, ChartRadarPrio> CHART_RADAR_PRIO_MAP
{ { "On Top"    , ChartRadarPrio::OVER }
, { "Supressed" , ChartRadarPrio::SUPP } };


// display category type
enum class ChartDisplayCat {
  DISPLAYBASE          = 'D'  //
, STANDARD             = 'S'  //
, OTHER                = 'O'  // O for OTHER
, MARINERS_STANDARD    = 'M'  // Mariner specified
, MARINERS_OTHER              // value not defined
};


static const QMap<QString, ChartDisplayCat> CHART_DISPLAY_CAT_MAP
{ { "Displaybase" , ChartDisplayCat::DISPLAYBASE }
, { "Standard"    , ChartDisplayCat::STANDARD }
, { "Other"       , ChartDisplayCat::OTHER }
, { "Mariners"    , ChartDisplayCat::MARINERS_STANDARD } };




#define MASK_POINT      1
#define MASK_LINE       2
#define MASK_AREA       4
#define MASK_MPS        8
#define MASK_ALL        MASK_POINT + MASK_LINE + MASK_AREA + MASK_MPS

// Rasterization rule types
enum class RastRuleType {
  NONE     // no rule type (init)
, TXT_TX   // TX
, TXT_TE   // TE
, SYM_PT   // SY
, SIM_LN   // LS
, COM_LN   // LC
, ARE_CO   // AC
, ARE_PA   // AP
, CND_SY   // CS
, MUL_SG   // Multipoint Sounding
, ARC_2C   // Circular Arc, used for sector lights, opencpn private
};

static const QMap<QString, RastRuleType> RAST_RULE_TYPE_MAP
{ { "CA" , RastRuleType::ARC_2C }
, { "MP" , RastRuleType::MUL_SG }
, { "TX" , RastRuleType::TXT_TX }
, { "TE" , RastRuleType::TXT_TE }
, { "SY" , RastRuleType::SYM_PT }
, { "LS" , RastRuleType::SIM_LN }
, { "LC" , RastRuleType::COM_LN }
, { "AC" , RastRuleType::ARE_CO }
, { "AP" , RastRuleType::ARE_PA }
, { "CS" , RastRuleType::CND_SY } };




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
  QSize     size;
  QVector2D distance;
  QPoint    pivot;
  QPoint    origin;
  QPoint    graphics_location;
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
