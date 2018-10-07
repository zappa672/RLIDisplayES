#ifndef INFOBLOCK_H
#define INFOBLOCK_H

#include <QRect>
#include <QColor>
#include <QMap>
#include <QDebug>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "infofonts.h"
#include "../../common/rlilayout.h"
#include "../../common/rlistrings.h"

struct InfoRect {  
  QColor color;
  QRect  geometry;
};

struct InfoText {
  RLIString   string;
  QByteArray  value;

  QString font_tag;
  QColor  color;
  QRect   geometry;
  RLITextAllign allign;

  InfoText() {
    string = RLI_STR_NONE;
    allign = RLI_ALLIGN_CENTER;
  }
};


class InfoBlock : protected QOpenGLFunctions {

public:
  explicit InfoBlock(const RLIInfoPanelLayout& layout, const std::map<QString, int>& text_id_map, QOpenGLContext* context);
  virtual ~InfoBlock();

  void clear();
  void resize(const RLIInfoPanelLayout& layout, const std::map<QString, int>& text_id_map);


  inline const QMap<QString, InfoRect>& rectangles() { return _rects; }
  inline const QVector<InfoText>& texts()      { return _texts; }


  inline const QRect& geometry()              { return _geometry; }

  inline bool needUpdate()                    { return _need_update; }
  inline void clearUpdate()                   { _need_update = false; }

  inline void setBackColor(const QColor& c)   { _back_color = c; _need_update = true; }
  inline const QColor& backColor()            { return _back_color; }

  inline void setBorderWidth(int w)           { _border_width = w; _need_update = true; }
  inline int borderWidth()                    { return _border_width; }

  inline void setBorderColor(const QColor& c) { _border_color = c; _need_update = true; }
  inline const QColor& borderColor()          { return _border_color; }


  QOpenGLFramebufferObject* fbo()             { return _fbo; }


  void setRect(QString rectId, const QRect& r);
  void setText(int textId, RLIString str);
  void setText(int textId, const QByteArray& val);

private:
  QMap<QString, InfoRect> _rects;
  QVector<InfoText> _texts;

  QRect   _geometry;
  QColor  _back_color;
  int     _border_width;
  QColor  _border_color;

  bool    _need_update;
  QOpenGLFramebufferObject* _fbo;
};


#endif // INFOBLOCK_H
