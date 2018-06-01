#ifndef S52ASSETS_H
#define S52ASSETS_H

#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include <QVector2D>
#include <QMap>

#include "s52references.h"

class S52Assets : protected QOpenGLFunctions
{
public:
  explicit S52Assets(QOpenGLContext* context, S52References* ref);
  virtual ~S52Assets();

  //inline QOpenGLTexture* getFontTexId       ()                                    { return font_texture; }

  inline QOpenGLTexture* getColorSchemeTex  (const QString& s)                    { return color_scheme_textures[s]; }

  // Returns patterns texture id for the color scheme
  inline QOpenGLTexture* getAreaPatternTex  (const QString& s)                    { return pattern_textures[s]; }
  // Returns pattern's left-top pixel location in patterns texture, s - color scheme, n - pattern tag
  inline QPoint getAreaPatternLocation      (const QString& s, const QString& n)  { return pat_lc[s].value(n, QPoint(-1, -1)); }
  // Returns size of the pattern
  inline QSize getAreaPatternSize           (const QString& s, const QString& n)  { return pat_sz[s].value(n, QSize(0, 0)); }

  inline QOpenGLTexture* getLinePatternTex  (const QString& s)                    { return line_textures[s]; }
  inline QPoint getLinePatternLocation      (const QString& s, const QString& n)  { return line_lc[s].value(n, QPoint(-1, -1)); }
  inline QSize getLinePatternSize           (const QString& s, const QString& n)  { return line_sz[s].value(n, QSize(0, 0)); }

  inline QOpenGLTexture* getSymbolTex       (const QString& s)                    { return symbol_textures[graphic_files[s]]; }

private:
  //void initFontTexture();

  void initColorSchemeTextures(S52References* ref);
  void initPatternTextures(S52References* ref);
  void initLineTextures(S52References* ref);
  void initSymbolTextures(S52References* ref);

  QOpenGLTexture* dirToPatternTex(const QString& path, const QString& ex_path, QMap<QString, QPoint>& locations,  QMap<QString, QSize>& sizes);

  //QOpenGLTexture*                         font_texture;

  QMap<QString, QOpenGLTexture*>          color_scheme_textures;

  QMap<QString, QOpenGLTexture*>          pattern_textures;
  QMap<QString, QMap<QString, QPoint>>    pat_lc;
  QMap<QString, QMap<QString, QSize>>     pat_sz;

  QMap<QString, QOpenGLTexture*>          line_textures;
  QMap<QString, QMap<QString, QPoint>>    line_lc;
  QMap<QString, QMap<QString, QSize>>     line_sz;

  QMap<QString, QString>                  graphic_files;
  QMap<QString, QOpenGLTexture*>          symbol_textures;
};

#endif // S52ASSETS_H
