#include "infofonts.h"

#include <QPixmap>
#include <QDebug>
#include <QDir>

InfoFonts::InfoFonts(QOpenGLContext* context, const QString& dirPath) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  QDir dir(dirPath);
  dir.setNameFilters(QStringList() << "*.png");

  for (QString fileName : dir.entryList()) {
    QString tag = fileName.replace(".png", "");

    QOpenGLTexture* tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
    QImage img(dir.absoluteFilePath(fileName));
    QSize fontSize = img.size() / 16;

    tex->setMipLevels(1);
    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Nearest);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    tex->setData(img, QOpenGLTexture::DontGenerateMipMaps);

    _textures.insert(tag, tex);
    _fontSizes.insert(tag, fontSize);
  }
}

InfoFonts::~InfoFonts() {
  //for (QOpenGLTexture* tex : _textures)
  //  tex->destroy();
}
