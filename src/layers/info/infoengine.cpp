/*


#include "infoengine.h"
#include "../../common/properties.h"

InfoEngine::InfoEngine(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context)  {
  initializeOpenGLFunctions();

  _context = context;
  _prog = new QOpenGLShaderProgram();

  _lang = RLI_LANG_RUSSIAN;

  decUTF8 = QTextCodec::codecForName("UTF8")->makeDecoder();
  decCP866 = QTextCodec::codecForName("cp866")->makeDecoder();

  _full_update = true;

  glGenBuffers(INFO_ATTR_COUNT, _vbo_ids);
  initShaders();
}

InfoEngine::~InfoEngine() {
  delete _prog;
  for (InfoBlock* block : _blocks)
    delete block;
  glDeleteBuffers(INFO_ATTR_COUNT, _vbo_ids);
}

InfoBlock* InfoEngine::addInfoBlock() {
  InfoBlock* block = new InfoBlock(_context, this);
  _blocks.push_back(block);
  return block;
}

void InfoEngine::onLanguageChanged(const QByteArray& lang) {
  QString lang_str = decCP866->toUnicode(lang);

  if (_lang == RLI_LANG_RUSSIAN && (lang_str == decUTF8->toUnicode(RLIStrings::nEng[RLI_LANG_RUSSIAN])
                                 || lang_str == decUTF8->toUnicode(RLIStrings::nEng[RLI_LANG_ENGLISH]))) {
      _lang = RLI_LANG_ENGLISH;
      _full_update = true;
  }

  if (_lang == RLI_LANG_ENGLISH && (lang_str == decUTF8->toUnicode(RLIStrings::nRus[RLI_LANG_ENGLISH])
                                 || lang_str == decUTF8->toUnicode(RLIStrings::nRus[RLI_LANG_RUSSIAN]))) {
      _lang = RLI_LANG_RUSSIAN;
      _full_update = true;
  }
}

void InfoEngine::update(InfoFonts* fonts) {
  glEnable(GL_BLEND);

  for (InfoBlock* block : _blocks)
    if (_full_update || block->needUpdate()) {
      block->fbo()->bind();
      _prog->bind();

      QRect geom = block->geometry();

      glViewport(0, 0, geom.width(), geom.height());

      QMatrix4x4 projection;
      projection.setToIdentity();
      projection.ortho(0.f, geom.width(), 0.f, geom.height(), -1.f, 1.f);

      _prog->setUniformValue(_uniform_locs[INFO_UNIF_MVP], projection);

      updateBlock(block, fonts);

      _prog->release();
      block->fbo()->release();
    }

  _full_update = false;
}

void InfoEngine::updateBlock(InfoBlock* b, InfoFonts* fonts) {
  QColor bckCol = b->backColor();
  QColor brdCol = b->borderColor();
  int    brdWid = b->borderWidth();
  QRect  geom   = QRect(QPoint(0, 0), b->geometry().size());

  glClearColor(bckCol.redF(), bckCol.greenF(), bckCol.blueF(), bckCol.alphaF());
  glClear(GL_COLOR_BUFFER_BIT);

  if (brdWid >= 1) {
    drawRect(QRect(QPoint(0, 0), QSize(geom.width(), brdWid)), brdCol);
    drawRect(QRect(QPoint(0, 0), QSize(brdWid, geom.height())), brdCol);
    drawRect(QRect(QPoint(0, geom.height() - brdWid), QSize(geom.width(), brdWid)), brdCol);
    drawRect(QRect(QPoint(geom.width() - brdWid, 0), QSize(brdWid, geom.height())), brdCol);
  }

  for (int i = 0; i < b->rectCount(); i++)
    drawRect(b->getRect(i).rect, b->getRect(i).col);

  for (int i = 0; i < b->textCount(); i++)
    drawText(b->getText(i), fonts);

  b->discardUpdate();
}

void InfoEngine::drawText(const InfoText& text, InfoFonts* fonts) {
  GLuint tex_id = fonts->getTexture(text.font_tag)->textureId();
  QSize font_size = fonts->getFontSize(text.font_tag);

  std::vector<GLfloat> pos;
  std::vector<GLfloat> ord, chars;

  QPoint anchor;
  switch (text.allign) {
    case INFOTEXT_ALLIGN_LEFT:
      anchor = text.rect.topLeft();
      break;
    case INFOTEXT_ALLIGN_RIGHT:
      anchor = text.rect.topRight() - QPoint(font_size.width()*text.str[_lang].size(), 0);
      break;
    case INFOTEXT_ALLIGN_CENTER:
      anchor = text.rect.center() - QPoint((font_size.width()*text.str[_lang].size()) / 2 - 1, font_size.height() / 2 - 1);
      break;
    default:
      anchor = text.rect.topLeft();
      break;
  }


  for (int i = 0; i < text.str[_lang].size(); i++) {
    QPoint lefttop = anchor + QPoint(i * font_size.width(), 0);
    for (int j = 0; j < 4; j++) {
      pos.push_back(lefttop.x());
      pos.push_back(lefttop.y());
      ord.push_back(j);
      chars.push_back(text.str[_lang][i]);
    }
  }


  glUniform2f(_uniform_locs[INFO_UNIF_SIZE], font_size.width(), font_size.height());
  glUniform4f(_uniform_locs[INFO_UNIF_COLOR], text.color.redF(), text.color.greenF(), text.color.blueF(), 1.f);


  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, ord.size()*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, chars.size()*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, ord.size());
  glBindTexture(GL_TEXTURE_2D, 0);
}


void InfoEngine::drawRect(const QRect& rect, const QColor& col) {
  std::vector<GLfloat> pos;

  pos.push_back(rect.x());
  pos.push_back(rect.y());
  pos.push_back(rect.x());
  pos.push_back(rect.y() + rect.height());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y() + rect.height());

  glUniform2f(_uniform_locs[INFO_UNIF_SIZE], 0.f, 0.f);
  glUniform4f(_uniform_locs[INFO_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, (void*) (0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glVertexAttrib1f(_attr_locs[INFO_ATTR_ORDER], 0.f);
  glDisableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glVertexAttrib1f(_attr_locs[INFO_ATTR_CHAR_VAL], 0.f);
  glDisableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void InfoEngine::initShaders() {
  _prog->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "info.vert.glsl");
  _prog->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "info.frag.glsl");
  _prog->link();
  _prog->bind();

  _attr_locs[INFO_ATTR_POSITION]  = _prog->attributeLocation("position");
  _attr_locs[INFO_ATTR_ORDER]     = _prog->attributeLocation("order");
  _attr_locs[INFO_ATTR_CHAR_VAL]  = _prog->attributeLocation("char_val");

  _uniform_locs[INFO_UNIF_MVP]    = _prog->uniformLocation("mvp_matrix");
  _uniform_locs[INFO_UNIF_COLOR]  = _prog->uniformLocation("color");
  _uniform_locs[INFO_UNIF_SIZE]   = _prog->uniformLocation("size");

  _prog->release();
}

*/
