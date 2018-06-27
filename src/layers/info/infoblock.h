#ifndef INFOBLOCK_H
#define INFOBLOCK_H

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
#include "../../common/rlistrings.h"

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
  inline const QRect& geometry()            { return _geometry; }
  void setBackColor(const QColor& c)        { _back_color = c; _need_update = true; }
  inline const QColor& backColor()          { return _back_color; }
  void setBorder(int w, const QColor& c)    { _border_width = w; _border_color = c; _need_update = true; }
  inline const QColor& borderColor()        { return _border_color; }
  inline int borderWidth()                  { return _border_width; }

  inline bool needUpdate()                  { return _need_update; }
  inline void discardUpdate()               { _need_update = false; }

  int addRect(const InfoRect& t)            { _rects.push_back(t); _need_update = true; return _rects.size() - 1; }
  int addText(const InfoText& t)            { _texts.push_back(t); _need_update = true; return _texts.size() - 1; }

  inline int rectCount()                    { return _rects.size(); }
  inline const InfoRect& getRect(int i)     { return _rects[i]; }
  inline int textCount()                    { return _texts.size(); }
  inline const InfoText& getText(int i)     { return _texts[i]; }

  inline GLuint texture()                   { return _fbo->texture(); }
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


#endif // INFOBLOCK_H
