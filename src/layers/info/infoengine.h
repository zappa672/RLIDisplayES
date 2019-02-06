#ifndef INFOENGINE_H
#define INFOENGINE_H

#include <map>

#include <QRect>
#include <QColor>
#include <QVector>
#include <QDebug>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "infofonts.h"
#include "infoblock.h"
#include "../../common/rlilayout.h"
#include "../../common/rlistrings.h"
#include "../../common/radarscale.h"
#include "../targetengine.h"


class InfoEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  InfoEngine (RLILayout* layout, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~InfoEngine ();

  void resize(RLILayout* layout);
  void update(InfoFonts* fonts);

  enum RLIPanel : unsigned char {
    RLI_PANEL_GAIN      = 0
  , RLI_PANEL_WATER     = 1
  , RLI_PANEL_RAIN      = 2
  , RLI_PANEL_APCH      = 3
  , RLI_PANEL_EMISSION  = 4

  , RLI_PANEL_LABEL5    = 5
  , RLI_PANEL_BAND      = 6
  , RLI_PANEL_ORIENTATION    = 7
  , RLI_PANEL_LABEL2    = 8
  , RLI_PANEL_LABEL3    = 9
  , RLI_PANEL_LABEL4    = 10
  , RLI_PANEL_FPS       = 11

  , RLI_PANEL_SCALE     = 12
  , RLI_PANEL_VN        = 13
  , RLI_PANEL_VD        = 14
  , RLI_PANEL_COURSE    = 15

  , RLI_PANEL_POSITION  = 16
  , RLI_PANEL_BLANK     = 17
  , RLI_PANEL_CLOCK     = 18

  , RLI_PANEL_DANGER    = 19
  , RLI_PANEL_TAILS     = 20
  , RLI_PANEL_DANGER_DETAILS = 21
  , RLI_PANEL_VECTOR    = 22
  , RLI_PANEL_TARGETS   = 23
  , RLI_PANEL_CURSOR    = 24

  , RLI_PANELS_COUNT    = 25
  };

  inline const QVector<InfoBlock*>& blocks() { return _blocks; }


public slots:
  void onLanguageChanged(RLIString lang_str);

  void updateGain(float gain);
  void updateWater(float water);
  void updateRain(float rain);
  void updateApch(float apch);
  void updateEmission(float emission);

  void updateValueBar(RLIPanel id, float value);

  void secondChanged();
  void setFps(int fps);

  void onCourseChanged(double course);
  void onPositionChanged(const GeoPos& position);
  void onTargetCountChanged(int count);
  void onSelectedTargetUpdated(const QString& tag, const RLITarget& trgt);
  void onScaleChanged(const rli_scale_t* scale);
  void onOrientationChanged(RLIOrientation orient);

private:
  void updateBlock(InfoBlock* b, InfoFonts* fonts);

  inline void drawText(const InfoText& text, InfoFonts* fonts);
  inline void drawRect(const QRect& rect, const QColor& col);

  void initBlocks();

  void initBlockGain();
  void initBlockWater();
  void initBlockRain();
  void initBlockApch();
  void initBlockEmission();

  void initBlockLabel5();
  void initBlockBand();
  void initBlockOrientation();
  void initBlockLabel2();
  void initBlockLabel3();
  void initBlockLabel4();

  void initBlockFps();
  void initBlockScale();
  void initBlockVn();
  void initBlockVd();
  void initBlockCourse();
  void initBlockPosition();
  void initBlockBlank();
  void initBlockClock();
  void initBlockDanger();
  void initBlockTails();
  void initBlockDetails();
  void initBlockVector();
  void initBlockTargets();
  void initBlockCursor();


  RLILang _lang = RLI_LANG_RUSSIAN;
  bool _full_update = true;

  QVector<InfoBlock*> _blocks = QVector<InfoBlock*>(RLI_PANELS_COUNT);

  void initShaders();

  // Info shader program
  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------
  enum { INFO_ATTR_POSITION = 0
       , INFO_ATTR_ORDER = 1
       , INFO_ATTR_CHAR_VAL = 2
       , INFO_ATTR_COUNT = 3 } ;
  enum { INFO_UNIF_MVP = 0
       , INFO_UNIF_COLOR = 1
       , INFO_UNIF_SIZE = 2
       , INFO_UNIF_COUNT = 3 } ;

  GLuint _vbo_ids[INFO_ATTR_COUNT];
  GLuint _attr_locs[INFO_ATTR_COUNT];
  GLuint _uniform_locs[INFO_UNIF_COUNT];
};

#endif // INFOENGINE_H

