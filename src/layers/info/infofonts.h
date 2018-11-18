#ifndef INFOFONTS_H
#define INFOFONTS_H

#include <QSize>
#include <QHash>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

class InfoFonts : protected QOpenGLFunctions
{
public:
  InfoFonts(QOpenGLContext* context, const QString& dirPath);
  virtual ~InfoFonts();

  inline QOpenGLTexture* getTexture(const QString& tag)   { return _textures.value(tag, nullptr); }
  inline QSize           getFontSize(const QString& tag)  { return _fontSizes.value(tag, QSize(0, 0)); }

private:
  QHash<QString, QOpenGLTexture*> _textures;
  QHash<QString, QSize> _fontSizes;
};

#endif // INFOFONTS_H
