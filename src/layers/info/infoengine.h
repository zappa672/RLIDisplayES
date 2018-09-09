#ifndef INFOENGINE_H
#define INFOENGINE_H

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
#include "../targetengine.h"


class InfoEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit InfoEngine (RLILayout* layout, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~InfoEngine ();

  void resize(RLILayout* layout);
  inline const QMap<QString, InfoBlock*>& blocks() { return _blocks; }
  void update(InfoFonts* fonts);

public slots:
  void onLanguageChanged(RLIString lang_str);

  void updateGain(float gain);
  void updateWater(float water);
  void updateRain(float rain);
  void updateApch(float apch);
  void updateEmission(float emission);

  void updateValueBar(const QString& name, float value);

  void secondChanged();
  void setFps(int fps);
  void onPositionChanged(const QVector2D& position);
  void onTargetCountChanged(int count);
  void onSelectedTargetUpdated(const QString& tag, const RLITarget& trgt);

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
  void initBlockLabel1();
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

  RLILang _lang;
  bool _full_update;

  QMap<QString, InfoBlock*> _blocks;

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


/*

#ifndef INFOENGINE_H
#define INFOENGINE_H

#include <QRect>
#include <QColor>
#include <QVector>
#include <QDebug>

#include <QTextEncoder>
#include <QTextDecoder>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "infofonts.h"
#include "infoblock.h"
#include "../../common/rlistrings.h"


class InfoEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit InfoEngine   (QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~InfoEngine   ();

  inline int blockCount() { return _blocks.size(); }
  inline int blockTextId(int b_id) { return _blocks[b_id]->texture(); }
  inline const QRect& blockGeometry(int b_id) { return _blocks[b_id]->geometry(); }

  InfoBlock* addInfoBlock();

public slots:
  void update(InfoFonts* fonts);
  void onLanguageChanged(const QByteArray& lang);

private:
  void updateBlock(InfoBlock* b, InfoFonts* fonts);
  inline void drawText(const InfoText& text, InfoFonts* fonts);
  inline void drawRect(const QRect& rect, const QColor& col);

  QOpenGLContext* _context;
  RLILang _lang;
  bool _full_update;
  QVector<InfoBlock*> _blocks;

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

  QTextDecoder* decUTF8;
  QTextDecoder* decCP866;
};

#endif // INFOENGINE_H

*/
