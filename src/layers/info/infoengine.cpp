#include "infoengine.h"

#include "../../common/properties.h"


InfoEngine::InfoEngine(RLILayout* layout, QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context)  {
  initializeOpenGLFunctions();

  _prog = new QOpenGLShaderProgram();
  _lang = RLI_LANG_RUSSIAN;

  for (const QString& name : layout->panels.keys())
    _blocks.insert(name, new InfoBlock(layout->panels[name], context));

  _full_update = true;

  glGenBuffers(INFO_ATTR_COUNT, _vbo_ids);
  initShaders();
  initBlocks();
}

InfoEngine::~InfoEngine() {
  delete _prog;
  for (InfoBlock* block : _blocks)
    delete block;
  glDeleteBuffers(INFO_ATTR_COUNT, _vbo_ids);
}

void InfoEngine::resize(RLILayout* layout) {
  for (const QString& name : _blocks.keys())
    _blocks[name]->resize(layout->panels[name]);
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

  for (const InfoRect& rect : b->rectangles())
    drawRect(rect.geometry, rect.color);

  for (const InfoText& text : b->texts())
    drawText(text, fonts);

  b->clearUpdate();
}

void InfoEngine::drawText(const InfoText& text, InfoFonts* fonts) {
  GLuint tex_id = fonts->getTexture(text.font_tag)->textureId();
  QSize font_size = fonts->getFontSize(text.font_tag);

  std::vector<GLfloat> pos;
  std::vector<GLfloat> ord, chars;

  QByteArray str;
  if (text.string == RLI_STR_NONE)
    str = text.value;
  else
    str = RLIStrings::instance().string(_lang, text.string);

  QPoint anchor;
  switch (text.allign) {
    case RLI_ALLIGN_LEFT:
      anchor = text.geometry.topLeft();
      break;
    case RLI_ALLIGN_RIGHT:
      anchor = text.geometry.topRight() - QPoint(font_size.width()*str.size(), 0);
      break;
    case RLI_ALLIGN_CENTER:
      anchor = text.geometry.center() - QPoint((font_size.width()*str.size()) / 2 - 1, font_size.height() / 2 - 1);
      break;
    default:
      anchor = text.geometry.topLeft();
      break;
  }

  for (int i = 0; i < str.size(); i++) {
    QPoint lefttop = anchor + QPoint(i * font_size.width(), 0);
    for (int j = 0; j < 4; j++) {
      pos.push_back(lefttop.x());
      pos.push_back(lefttop.y());
      ord.push_back(j);
      chars.push_back(str[i]);
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

void InfoEngine::onLanguageChanged(RLIString lang_str) {
  if (lang_str == RLI_STR_ARRAY_LANG_ENGL)
    _lang = RLI_LANG_ENGLISH;

  if (lang_str == RLI_STR_ARRAY_LANG_RUS)
    _lang = RLI_LANG_RUSSIAN;

  _full_update = true;
}




void InfoEngine::initBlocks() {
  initBlockGain();
  initBlockWater();
  initBlockRain();
  initBlockApch();
  initBlockEmission();

  initBlockLabel5();
  initBlockBand();
  initBlockLabel1();
  initBlockLabel2();
  initBlockLabel3();
  initBlockLabel4();

  initBlockFps();
  initBlockScale();
  initBlockVn();
  initBlockVd();
  initBlockCourse();
  initBlockPosition();
  initBlockBlank();
  initBlockClock();
  initBlockDanger();
  initBlockTails();
  initBlockDetails();
  initBlockVector();
  initBlockTargets();
  initBlockCursor();
}


void InfoEngine::initBlockGain() {
  _blocks["gain"]->setText("text", RLI_STR_GAIN);
  QRect bar = _blocks["gain"]->rectangles()["bar"].geometry;
  bar.setWidth(40);
  _blocks["gain"]->setRect("bar", bar);
}

void InfoEngine::initBlockWater() {
  _blocks["water"]->setText("text", RLI_STR_WATER);
  QRect bar = _blocks["water"]->rectangles()["bar"].geometry;
  bar.setWidth(30);
  _blocks["water"]->setRect("bar", bar);
}

void InfoEngine::initBlockRain() {
  _blocks["rain"]->setText("text", RLI_STR_RAIN);
  QRect bar = _blocks["rain"]->rectangles()["bar"].geometry;
  bar.setWidth(20);
  _blocks["rain"]->setRect("bar", bar);
}

void InfoEngine::initBlockApch() {
  _blocks["apch"]->setText("text", RLI_STR_AFC);
  QRect bar = _blocks["apch"]->rectangles()["bar"].geometry;
  bar.setWidth(0);
  _blocks["apch"]->setRect("bar", bar);
}

void InfoEngine::initBlockEmission() {
  _blocks["emission"]->setText("text", RLI_STR_EMISSION);
  QRect bar = _blocks["emission"]->rectangles()["bar"].geometry;
  bar.setWidth(49);
  _blocks["emission"]->setRect("bar", bar);
}


//<panel name="label5" pos="4,104" size="102x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="text" rect="0x0x102x20" allign="center" font="F12X14B" color="0,252,252,255" />
//</panel>
void InfoEngine::initBlockLabel5() {

}

//<panel name="band" pos="4,128" size="102x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="text" rect="0x0x102x20" allign="center" font="F12X14B" color="0,252,252,255" />
//</panel>
void InfoEngine::initBlockBand() {

}

//<panel name="label1" pos="-244,44" size="112x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="text" rect="0x0x112x20" allign="center" font="F12X14B" color="0,252,252,255" />
//</panel>
void InfoEngine::initBlockLabel1() {

}

//<panel name="label2" pos="-244,68" size="112x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="text" rect="0x0x112x20" allign="center" font="F12X14B" color="0,252,252,255" />
//</panel>
void InfoEngine::initBlockLabel2() {

}

//<panel name="label3" pos="-244,92" size="112x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="text" rect="0x0x112x20" allign="center" font="F12X14B" color="0,252,252,255" />
//</panel>
void InfoEngine::initBlockLabel3() {

}

//<panel name="label4" pos="-244,-68" size="96x32" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="text" rect="0x0x112x20" allign="center" font="F16X28B" color="0,252,252,255" />
//</panel>
void InfoEngine::initBlockLabel4() {

}


void InfoEngine::initBlockFps() {
  _blocks["fps"]->setText("label", RLI_STR_FPS);
  _blocks["fps"]->setText("value", QString::number(0).toLatin1());
}

//<!-- rect="left_x_top_x_width_x_height" -->
//<panel name="scale" pos="-244,4" size="236x36" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <rect name="splitter" rect="174x0x1x36" color="64,252,0,255" />
//  <!-- /////////////////////////////////////////////////////////// -->
//  <text name="scale1" rect="5x5x80x28"    allign="center" font="F16X28B" color="252,252,84,255" />  <!-- dynamic text color -->
//  <text name="slash"  rect="85x5x16x28"   allign="center" font="F16X28B" color="252,252,84,255" />  <!-- dynamic text color -->
//  <text name="scale2" rect="101x16x70x14" allign="center" font="F14X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//  <text name="units"  rect="176x12x56x14" allign="center" font="F14X14B" color="0,252,252,255"  />  <!-- static text color -->
//</panel>
void InfoEngine::initBlockScale() {

}

//<panel name="vn" pos="4,-4" size="140x66" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="header" rect="0x6x132x14" allign="center" font="F12X14B" color="0,252,252,255" /> <!-- static text color -->
//  <!-- /////////////////////////////////////////////////////////// -->
//  <table name="table" row_count="2" top="28" row_height="18" >
//    <column left="4"    width="36" allign="left"  font="F12X14B"  color="0,252,252,255"   />  <!-- static text color -->
//    <column left="40"   width="60" allign="right" font="F12X14B"  color="252,252,84,255"  />  <!-- dynamic text color -->
//    <column left="100"  width="36" allign="left"  font="F8X14B"   color="0,252,252,255"   />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockVn() {

}

//<panel name="vd" pos="-244,-4" size="140x52" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="header" rect="0x6x132x14" allign="center" font="F12X14B" color="0,252,252,255" /> <!-- static text color -->
//  <!-- /////////////////////////////////////////////////////////// -->
//  <table name="table" row_count="1" top="28" row_height="18" >
//    <column left="4"    width="90" allign="right" font="F12X14B"  color="0,252,252,255"   />   <!-- static text color -->
//    <column left="100"  width="40" allign="left"  font="F8X14B"   color="252,252,84,255"  />   <!-- dynamic text color -->
//  </table>
//</panel>
void InfoEngine::initBlockVd() {

}

//<panel name="course" pos="-4,4" size="236x38" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <table name="table" row_count="2" top="4" row_height="18" >
//    <column left="4"    width="116" allign="left"   font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120"  width="60"  allign="right"  font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//    <column left="186"  width="36"  allign="left"   font="F8X14B"  color="0,252,252,255"  />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockCourse() {

}

//<panel name="position" pos="-4,46" size="236x38" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <table name="table" row_count="2" top="4" row_height="18" >
//    <column left="4"    width="116" allign="left"   font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120"  width="112" allign="right"  font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//  </table>
//</panel>
void InfoEngine::initBlockPosition() {

}

//<panel name="blank" pos="-4,88" size="236x38" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//</panel>
void InfoEngine::initBlockBlank() {

}

//<panel name="clock" pos="-4,130" size="236x18" border_width="0" border_color="0,0,0,255" back_color="0,0,0,0" >
//  <text name="label"  rect="4x4x80x14"    allign="center" font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//  <text name="time"   rect="120x4x96x14"  allign="center" font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//</panel>
void InfoEngine::initBlockClock() {

}

//<panel name="danger" pos="-4,-330" size="236x20" border_width="0" border_color="0,0,0,255" back_color="252,252,84,255" >
//  <text name="label" rect="0x0x236x20" allign="center" font="F12X14B" color="0,0,0,255"  />
//</panel>
void InfoEngine::initBlockDanger() {

}

//<panel name="tails" pos="-4,-306" size="236x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <table name="table" row_count="1" top="3" row_height="18" >
//    <column left="4"   width="116" allign="left"  font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120" width="60"  allign="right" font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//    <column left="186" width="36"  allign="left"  font="F8X14B"  color="0,252,252,255"  />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockTails() {

}

//<panel name="danger-details" pos="-4,-264" size="236x38" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <table name="table" row_count="2" top="3" row_height="18" >
//    <column left="4"   width="116" allign="left"  font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120" width="60"  allign="right" font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//    <column left="186" width="36"  allign="left"  font="F8X14B"  color="0,252,252,255"  />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockDetails() {

}

//<panel name="vector" pos="-4,-240" size="236x20" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <table name="table" row_count="1" top="3" row_height="18" >
//    <column left="4"   width="116" allign="left"  font="F12X14B"  color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120" width="60"  allign="right" font="F12X14B"  color="252,252,84,255" />  <!-- dynamic text color -->
//    <column left="186" width="36"  allign="left"  font="F8X14B"   color="0,252,252,255"  />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockVector() {

}

//<panel name="targets" pos="-4,-68" size="236x168" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <rect name="header-middle" rect="118x0x1x20" color="64,252,0,255" />
//  <rect name="header-bottom" rect="0x20x236x1" color="64,252,0,255" />
//  <!-- /////////////////////////////////////////////////////////// -->
//  <text name="header"         rect="4x4x228x14" allign="left"   font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//  <text name="current-target" rect="4x4x110x14" allign="right"  font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//  <text name="target-count"   rect="4x4x228x14" allign="right"  font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//  <!-- /////////////////////////////////////////////////////////// -->
//  <table name="table" row_count="8" top="24" row_height="18" >
//    <column left="4"   width="116" allign="left"  font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120" width="60"  allign="right" font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//    <column left="186" width="36"  allign="left"  font="F8X14B"  color="0,252,252,255"  />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockTargets() {

}

//<panel name="cursor" pos="-4,-4" size="236x60" border_width="1" border_color="64,252,0,255" back_color="0,0,0,255" >
//  <text name="header" rect="0x4x236x14" allign="center" font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//  <!-- /////////////////////////////////////////////////////////// -->
//  <table name="table" row_count="2" top="24" row_height="18" >
//    <column left="4"   width="116" allign="left"  font="F12X14B" color="0,252,252,255"  />  <!-- static text color -->
//    <column left="120" width="60"  allign="right" font="F12X14B" color="252,252,84,255" />  <!-- dynamic text color -->
//    <column left="186" width="36"  allign="left"  font="F8X14B"  color="0,252,252,255"  />  <!-- static text color -->
//  </table>
//</panel>
void InfoEngine::initBlockCursor() {

}
