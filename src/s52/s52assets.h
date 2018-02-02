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
  explicit S52Assets                     (QOpenGLContext* context, S52References* ref);
  virtual ~S52Assets                     ();

  inline GLuint    getFontTexId          ()                                           { return font_texture->textureId(); }

  inline QOpenGLTexture* getColorSchemeTex (const QString& scheme)                  { return color_scheme_textures[scheme]; }

  // Returns patterns texture id for the color scheme
  inline QOpenGLTexture* getPatternTex   (const QString& scheme)                      { return pattern_textures[scheme]; }
  // Returns pattern's left-top pixel location in patterns texture
  inline QPoint    getPatternLocation    (const QString& scheme, const QString& name) { return (pattern_locations[scheme].contains(name))
                                                                                              ? pattern_locations[scheme][name]
                                                                                              : QPoint(-1, -1); }
  // Returns size of the pattern
  inline QSize     getPatternSize        (const QString& scheme, const QString& name) { return (pattern_sizes[scheme].contains(name))
                                                                                              ? pattern_sizes[scheme][name]
                                                                                              : QSize(0, 0); }


  inline GLuint    getLineTextureId      (const QString& scheme)                      { return line_tex_ids[scheme]; }
  inline QVector2D getLineTextureDim     (const QString& scheme)                      { return line_tex_dims[scheme]; }
  inline QVector2D getLineIndex          (const QString& scheme, const QString& name) { return line_ind_maps[scheme][name]; }
  inline QVector2D getLineDim            (const QString& scheme, const QString& name) { return line_dim_maps[scheme][name]; }

  inline GLuint     getSymbolTextureId   (const QString& scheme)                      { return symbol_tex_ids[graphic_files[scheme]]; }
  inline QVector2D  getSymbolTextureDim  (const QString& scheme)                      { return symbol_tex_dims[graphic_files[scheme]]; }

private:
  void initGlyphTexture                  ();
  void initColorSchemeTextures           (S52References* ref);
  void initPatternTextures               (S52References* ref);
  void initLineTextures                  (S52References* ref);
  void initSymbolTextures                (S52References* ref);

  QOpenGLTexture*                       font_texture;

  QMap<QString, QOpenGLTexture*>        color_scheme_textures;

  QMap<QString, QOpenGLTexture*>        pattern_textures;
  QMap<QString, QMap<QString, QPoint>>  pattern_locations;
  QMap<QString, QMap<QString, QSize>>   pattern_sizes;

  QMap< QString, GLuint >                   line_tex_ids;
  QMap< QString, QVector2D >                line_tex_dims;
  QMap< QString, QMap<QString, QVector2D> > line_ind_maps;
  QMap< QString, QMap<QString, QVector2D> > line_dim_maps;

  QMap< QString, QString >                  graphic_files;
  QMap< QString, GLuint >                   symbol_tex_ids;
  QMap< QString, QVector2D >                symbol_tex_dims;
  QMap< QString, QMap<QString, QVector2D> > symbol_ind_maps;
  QMap< QString, QMap<QString, QVector2D> > symbol_dim_maps;
  QMap< QString, QMap<QString, QVector2D> > symbol_piv_maps;
};

#endif // S52ASSETS_H
