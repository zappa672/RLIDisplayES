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
#include "../common/rlistrings.h"

enum TextAllign { INFOTEXT_ALLIGN_LEFT, INFOTEXT_ALLIGN_RIGHT, INFOTEXT_ALLIGN_CENTER };

struct InfoRect {
  QColor col;
  QRect  rect;
};

struct InfoText {
  QByteArray str[RLI_LANG_COUNT];
  QString font_tag;
  QColor color;
  QRect rect;
  TextAllign allign;

  InfoText() { allign = INFOTEXT_ALLIGN_LEFT; }
};




class InfoBlock : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:

  explicit InfoBlock(QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~InfoBlock();

  void clear();

  void setGeometry(const QRect& r);
  inline const QRect& getGeometry()         { return _geometry; }
  void setBackColor(const QColor& c)        { _back_color = c; _need_update = true; }
  inline const QColor& getBackColor()       { return _back_color; }
  void setBorder(int w, const QColor& c)    { _border_width = w; _border_color = c; _need_update = true; }
  inline const QColor& getBorderColor()     { return _border_color; }
  inline int getBorderWidth()               { return _border_width; }

  inline bool needUpdate()                  { return _need_update; }
  inline void discardUpdate()               { _need_update = false; }

  int addRect(const InfoRect& t)            { _rects.push_back(t); _need_update = true; return _rects.size() - 1; }
  int addText(const InfoText& t)            { _texts.push_back(t); _need_update = true; return _texts.size() - 1; }

  inline int getRectCount()                 { return _rects.size(); }
  inline const InfoRect& getRect(int i)     { return _rects[i]; }
  inline int getTextCount()                 { return _texts.size(); }
  inline const InfoText& getText(int i)     { return _texts[i]; }

  inline GLuint getTextureId()              { return _fbo->texture(); }
  QOpenGLFramebufferObject* fbo()           { return _fbo; }

public slots:
  void setRect(int rectId, const QRect& r);
  void setText(int textId, int lang_id, const QByteArray& str);

private:
  QVector<InfoRect> _rects;
  QVector<InfoText> _texts;

  QRect   _geometry;
  QColor  _back_color;
  QColor  _border_color;
  int     _border_width;
  bool    _need_update;

  // Framebuffer vars
  QOpenGLFramebufferObject* _fbo;
};




class InfoEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit InfoEngine   (QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~InfoEngine   ();

  inline int getBlockCount() { return _blocks.size(); }
  inline int getBlockTextId(int b_id) { return _blocks[b_id]->getTextureId(); }
  inline const QRect& getBlockGeometry(int b_id) { return _blocks[b_id]->getGeometry(); }

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

  bool initShaders();

  // Info shader program
  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------
  enum { INFO_ATTR_POSITION = 0
       , INFO_ATTR_ORDER = 1
       , INFO_ATTR_CHAR_VAL = 2
       , INFO_ATTR_COUNT = 3 } ;
  enum { INFO_UNIFORM_COLOR = 0
       , INFO_UNIFORM_SIZE = 1
       , INFO_UNIFORM_COUNT = 2 } ;

  GLuint _vbo_ids[INFO_ATTR_COUNT];
  GLuint _attr_locs[INFO_ATTR_COUNT];
  GLuint _uniform_locs[INFO_UNIFORM_COUNT];

  QTextEncoder* enc;
  QTextDecoder* dec;
  QTextDecoder* dec1;
};

#endif // INFOENGINE_H
