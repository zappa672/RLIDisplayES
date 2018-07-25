#include "infoblock.h"

#include <QDebug>
#include <QDateTime>

InfoBlock::InfoBlock(const RLIInfoPanelLayout& layout, QOpenGLContext* context) : QOpenGLFunctions(context)  {
  initializeOpenGLFunctions();

  clear();
  _fbo = nullptr;
  resize(layout);

  _need_update = false;
}

InfoBlock::~InfoBlock() {
  delete _fbo;
}


void InfoBlock::setRect(int rectId, const QRect& rect) {
  _rects[rectId].geometry = rect;
}

void InfoBlock::setText(int textId, RLIString str) {
  _texts[textId].string = str;
  _texts[textId].value.clear();
}

void InfoBlock::setText(int textId, const QByteArray& val) {
  _texts[textId].string = RLI_STR_NONE;
  _texts[textId].value = val;
}




void InfoBlock::clear() {
  _geometry     = QRect(0, 0, 1, 1);
  _back_color   = QColor(1, 1, 1, 0);
  _border_color = QColor(1, 1, 1, 0);
  _border_width = 0;

  _texts.clear();
  _rects.clear();
}

void InfoBlock::resize(const RLIInfoPanelLayout& layout) {
  qDebug() << "InfoBlock::resize" << layout.name << layout.geometry;

  if (_fbo == nullptr || layout.geometry.size() != _fbo->size()) {
    delete _fbo;
    _fbo = new QOpenGLFramebufferObject(layout.geometry.size());
  }

  _geometry     = layout.geometry;
  _back_color   = layout.back_color;
  _border_color = layout.border_color;
  _border_width = layout.border_width;

  _need_update = true;
}
