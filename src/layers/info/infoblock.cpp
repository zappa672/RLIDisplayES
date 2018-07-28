#include "infoblock.h"

#include <QDebug>
#include <QDateTime>


InfoBlock::InfoBlock(const RLIInfoPanelLayout& layout, QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();
  clear();
  _fbo = nullptr;
  resize(layout);
  _need_update = false;
}

InfoBlock::~InfoBlock() {
  delete _fbo;
}


void InfoBlock::setRect(QString rectId, const QRect& rect) {
  _rects[rectId].geometry = rect;
  _need_update = true;
}

void InfoBlock::setText(QString textId, RLIString str) {
  _texts[textId].string = str;
  _texts[textId].value.clear();
  _need_update = true;
}

void InfoBlock::setText(QString textId, const QByteArray& val) {
  _texts[textId].string = RLI_STR_NONE;
  _texts[textId].value = val;
  _need_update = true;
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
  if (_fbo == nullptr || layout.geometry.size() != _fbo->size()) {
    delete _fbo;
    _fbo = new QOpenGLFramebufferObject(layout.geometry.size());
  }

  _geometry     = layout.geometry;
  _back_color   = layout.back_color;
  _border_color = layout.border_color;
  _border_width = layout.border_width;

  for (const RLIInfoRectLayout& rl : layout.rects) {
    InfoRect rect;
    rect.geometry = rl.rect;
    rect.color = rl.color;
    _rects[rl.name] = rect;
  }

  for (const RLIInfoTextLayout& tl : layout.texts) {
    InfoText text;
    text.geometry = tl.bounding_rect;
    text.color = tl.color;
    text.allign = tl.allign;
    text.font_tag = tl.font_tag;
    if (_texts.contains(tl.name)) {
      text.string = _texts[tl.name].string;
      text.value = _texts[tl.name].value;
    }
    _texts[tl.name] = text;
  }

  for (const RLIInfoTableLayout& tl : layout.tables) {
    for (int i = 0; i < tl.row_count; i++) {
      for (int j = 0; j < tl.columns.size(); j++) {
        QString text_name = tl.name + "_" + QString::number(i) + "_" + QString::number(j);

        QPoint topLeft(tl.columns[j].left, tl.top + i * tl.row_height);
        QSize size(tl.columns[j].width, tl.row_height);

        InfoText text;
        text.geometry = QRect(topLeft, size);
        text.color = tl.columns[j].color;
        text.allign = tl.columns[j].allign;
        text.font_tag = tl.columns[j].font_tag;

        if (_texts.contains(text_name)) {
          text.string = _texts[text_name].string;
          text.value = _texts[text_name].value;
        }

        _texts[text_name] = text;
      }
    }
  }

  _need_update = true;
}
