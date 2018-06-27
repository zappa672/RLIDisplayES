#include "infoblock.h"

#include <QDebug>
#include <QDateTime>

InfoBlock::InfoBlock(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context)  {
  initializeOpenGLFunctions();

  clear();
  _fbo = new QOpenGLFramebufferObject(_geometry.size());

  _need_update = false;
}

InfoBlock::~InfoBlock() {
  delete _fbo;
}

void InfoBlock::setRect(int rectId, const QRect& r) {
  if (rectId < _rects.size())
    _rects[rectId].rect = r;

  _need_update = true;
}

void InfoBlock::setText(int textId, int lang_id, const QByteArray& str) {
  if (textId < _texts.size())
    _texts[textId].str[lang_id] = str;

  _need_update = true;
}

void InfoBlock::clear() {
  _geometry = QRect(0, 0, 1, 1);
  _back_color = QColor(1, 1, 1, 0);
  _border_color = QColor(1, 1, 1, 0);
  _border_width = 0;

  _texts.clear();
  _rects.clear();
}

void InfoBlock::setGeometry(const QRect& r) {
  _geometry = r;
  _need_update = true;

  delete _fbo;
  _fbo = new QOpenGLFramebufferObject(_geometry.size());
}

