#include "s52assets.h"

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

  initFontTexture();

  initColorSchemeTextures(ref);

  initPatternTextures(ref);
  initLineTextures(ref);
  initSymbolTextures(ref);
}

S52Assets::~S52Assets() {
  font_texture->destroy();

  for (QOpenGLTexture* tex : pattern_textures)
    tex->destroy();

  for (QOpenGLTexture* tex : line_textures)
    tex->destroy();

  for (QOpenGLTexture* tex : symbol_textures)
    tex->destroy();

  for (QOpenGLTexture* tex : color_scheme_textures)
    tex->destroy();
}


void S52Assets::initFontTexture() {
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

  font_texture->setMipLevels(0);
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

    tex->setMipLevels(0);
    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);

    tex->setData(img);

    color_scheme_textures.insert(scheme, tex);
  }
}

QOpenGLTexture* S52Assets::dirToPatternTex(const QString& path, QMap<QString, QPoint>& locations, QMap<QString, QSize>& sizes) {
  QDir png_dir(path);
  QMap<QString, QImage> patterns;

  int total_width = 0;
  int max_height  = 0;

  // Load images from files to patterns map, fill locations and sizes
  for (QString file_name : png_dir.entryList(QStringList() << "*.png")) {
    QString pat_name = file_name.replace(".png", "");
    QImage pattern = QImage(png_dir.filePath(file_name));

    locations.insert(pat_name, QPoint(total_width, 0));
    sizes.insert(pat_name, pattern.size());

    total_width += pattern.width();
    if (pattern.height() > max_height)
      max_height = pattern.height();

    patterns.insert(pat_name, pattern);
  }

  // Combine patterns to a single image according to locations
  QImage img(total_width, max_height, QImage::Format_ARGB32);
  QPainter painter(&img);
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(img.rect(), Qt::transparent);

  for (QString pat_name : patterns.keys())
    painter.drawImage(QRect(locations[pat_name], sizes[pat_name]), patterns[pat_name]);

  // Create OpenGL texture using combined image
  QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);

  tex->setMipLevels(0);
  tex->setMinificationFilter(QOpenGLTexture::Nearest);
  tex->setMagnificationFilter(QOpenGLTexture::Nearest);
  tex->setWrapMode(QOpenGLTexture::Repeat);

  tex->setData(img);
  return tex;
}

void S52Assets::initPatternTextures(S52References* ref) {
  for (QString scheme : ref->getColorSchemeNames()) {
    QMap<QString, QPoint> locations;
    QMap<QString, QSize> sizes;

    QString dir_path = "data/textures/charts/patterns/" + scheme.toLower();
    QOpenGLTexture* tex = dirToPatternTex(dir_path, locations, sizes);

    pattern_textures.insert(scheme, tex);
    pat_lc.insert(scheme, locations);
    pat_sz.insert(scheme, sizes);
  }
}

void S52Assets::initLineTextures(S52References* ref) {
  for (QString scheme : ref->getColorSchemeNames()) {
    QMap<QString, QPoint> locations;
    QMap<QString, QSize> sizes;

    QString dir_path = "data/textures/charts/lines/" + scheme.toLower();
    QOpenGLTexture* tex = dirToPatternTex(dir_path, locations, sizes);

    line_textures.insert(scheme, tex);
    line_lc.insert(scheme, locations);
    line_sz.insert(scheme, sizes);
  }
}

void S52Assets::initSymbolTextures(S52References* ref) {
  for (QString scheme : ref->getColorSchemeNames()) {
    QString file_name = ref->getGraphicsFileName(scheme);

    graphic_files.insert(scheme, file_name);

    if (symbol_textures.contains(file_name))
      continue;

    QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
    QImage img("data/textures/charts/symbols/" + file_name);

    tex->setMipLevels(1);
    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);

    tex->setData(img);
    symbol_textures.insert(file_name, tex);
  }
}
