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
