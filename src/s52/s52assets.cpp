#include "s52assets.h"

#include <QDebug>

#include <QDir>
#include <QGLWidget>
#include <QStringList>

#include <QFontDatabase>
#include <QGraphicsTextItem>
#include <QTextCodec>
#include <QPixmap>
#include <QPainter>
#include <QRectF>


S52Assets::S52Assets(QOpenGLContext* context, S52References* ref) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  initGlyphTexture();

  initColorSchemeTextures(ref);

  initPatternTextures(ref);
  initLineTextures(ref);
  initSymbolTextures(ref);
}

S52Assets::~S52Assets() {
  font_texture->destroy();

  for (QOpenGLTexture* tex : pattern_textures)
    tex->destroy();

  for (int i = 0; i < line_tex_ids.keys().size(); i++)
    glDeleteTextures(1, &line_tex_ids[line_tex_ids.keys()[i]]);

  for (int i = 0; i < symbol_tex_ids.keys().size(); i++)
    glDeleteTextures(1, &symbol_tex_ids[symbol_tex_ids.keys()[i]]);

  for (QOpenGLTexture* tex : color_scheme_textures)
    tex->destroy();
}


void S52Assets::initGlyphTexture() {
  QImage img(32*16, 32*16, QImage::Format_ARGB32);

  int id = QFontDatabase::addApplicationFont(":/fonts/Helvetica.ttf");
  QString family = QFontDatabase::applicationFontFamilies(id).at(0);

  QPainter painter(&img);

  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.setPen(Qt::black);
  painter.setBrush(Qt::NoBrush);
  painter.setFont(QFont(family, 16, QFont::Normal));

  painter.fillRect(img.rect(), Qt::transparent);

  char char_table[256];
  QTextDecoder* dec = QTextCodec::codecForName("cp1251")->makeDecoder();
  for (int i = 0; i < 256; i++)
    char_table[i] = static_cast<char>(i);
  QString uchars = dec->toUnicode(char_table, 256);

  for (int i = 0; i < 256; i++)
    painter.drawText(QRect(32 * (i % 16), 32 * (i / 16), 32.f, 32.f), Qt::AlignCenter, uchars.at(i));


  font_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);

  font_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  font_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
  font_texture->setWrapMode(QOpenGLTexture::ClampToEdge);

  font_texture->setData(img);
}


void S52Assets::initColorSchemeTextures(S52References* ref) {
  QStringList color_scheme_names = ref->getColorSchemeNames();
  int colors_count = ref->getColorsCount();

  for (QString scheme : color_scheme_names) {
    ColorTable* c_tbl = ref->getColorTable(scheme);

    QImage img(1, colors_count, QImage::Format_RGB888);
    for (QString color_tag : c_tbl->colors.keys())
      img.setPixel(0, ref->getColorIndex(color_tag), c_tbl->colors[color_tag].rgb());

    QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);

    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);

    tex->setData(img);

    color_scheme_textures.insert(scheme, tex);
  }
}

void S52Assets::initPatternTextures(S52References* ref) {
  QStringList color_schemes = ref->getColorSchemeNames();

  for (QString scheme_name : color_schemes) {
    QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);

    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    QDir png_dir("data/textures/charts/patterns/" + scheme_name.toLower());
    png_dir.setNameFilters(QStringList() << "*.png");
    QStringList png_list = png_dir.entryList();

    // Templates are 128x128
    int cols = 4;
    int rows = (png_list.count() - 1) / cols + 1;

    QImage img(128*cols, 128*rows, QImage::Format_ARGB32);
    QPainter painter(&img);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(img.rect(), Qt::transparent);

    QMap<QString, QPoint> locations;
    QMap<QString, QSize> sizes;

    for (int i = 0; i < png_list.count(); i++) {
      QString file_name = png_list[i];
      QString pattern_name = file_name.replace(".png", "");
      QImage pattern = QImage(png_dir.filePath(file_name));

      QPoint location(128*(i % cols), 128*(i / cols));
      locations.insert(pattern_name, location);
      sizes.insert(pattern_name, pattern.size());

      painter.drawImage(QRect(location, pattern.size()), pattern);
    }

    tex->setData(img);
    pattern_textures.insert(scheme_name, tex);
    pattern_tex_sizes.insert(scheme_name, img.size());
    pattern_locations.insert(scheme_name, locations);
    pattern_sizes.insert(scheme_name, sizes);
  }
}

void S52Assets::initLineTextures(S52References* ref) {
  QStringList color_schemes = ref->getColorSchemeNames();

  GLuint* tex_ids = new GLuint[color_schemes.size()];
  glGenTextures(color_schemes.size(), tex_ids);

  for (int k = 0; k < color_schemes.size(); k++) {
    line_tex_ids.insert(color_schemes[k], tex_ids[k]);

    glBindTexture(GL_TEXTURE_2D, tex_ids[k]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    QDir simple_tex_dir("data/textures/charts/lines/simple");
    simple_tex_dir.setNameFilters(QStringList() << "*.png");
    QStringList simple_tex_list = simple_tex_dir.entryList();

    QDir color_tex_dir("data/textures/charts/lines/" + color_schemes[k].toLower());
    color_tex_dir.setNameFilters(QStringList() << "*.png");
    QStringList color_tex_list = color_tex_dir.entryList();

    int line_tex_count = simple_tex_list.size() + color_tex_list.size();

    QVector2D line_tex_dim;
    line_tex_dim.setX(128);
    line_tex_dim.setY(32*line_tex_count);
    line_tex_dims.insert(color_schemes[k], line_tex_dim);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, line_tex_dim.x(), line_tex_dim.y()
               , 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);

    QString tex_file_name;
    QImage img_tex;
    QMap<QString, QVector2D> line_ind_map;
    QMap<QString, QVector2D> line_dim_map;

    for (int i = 0; i < line_tex_count; i++) {
      if (i < simple_tex_list.size()) {
        tex_file_name = simple_tex_list[i];
        img_tex = QImage(simple_tex_dir.filePath(tex_file_name));
      } else {
        tex_file_name = color_tex_list[i - simple_tex_list.size()];
        img_tex = QImage(color_tex_dir.filePath(tex_file_name));
      }

      img_tex = QGLWidget::convertToGLFormat(img_tex);
      line_ind_map.insert(tex_file_name.replace(".png", ""), QVector2D(0, 32*i));
      line_dim_map.insert(tex_file_name.replace(".png", ""), QVector2D(img_tex.width(), img_tex.height()));

      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i*32, img_tex.width(), img_tex.height()
                    , GL_RGBA, GL_UNSIGNED_BYTE, img_tex.bits());
    }

    line_ind_maps.insert(color_schemes[k], line_ind_map);
    line_dim_maps.insert(color_schemes[k], line_dim_map);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}


void S52Assets::initSymbolTextures(S52References* ref) {
  QStringList color_schemes = ref->getColorSchemeNames();

  GLuint symbol_tex_id;
  for (int k = 0; k < color_schemes.size(); k++) {
    QString file_name = ref->getGraphicsFileName(color_schemes[k]);
    graphic_files.insert(color_schemes[k], file_name);

    if (symbol_tex_ids.contains(file_name))
      continue;

    glGenTextures(1, &symbol_tex_id);
    symbol_tex_ids.insert(file_name, symbol_tex_id);

    glBindTexture(GL_TEXTURE_2D, symbol_tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    QImage img("data/textures/charts/symbols/" + file_name);
    img = QGLWidget::convertToGLFormat(img);
    symbol_tex_dims.insert(file_name, QVector2D(img.width(), img.height()));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height()
                 , 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
