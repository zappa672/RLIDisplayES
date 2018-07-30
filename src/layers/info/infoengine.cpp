#include "infoengine.h"

#include <QTime>

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
  initBlocks();
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
  bar.setWidth(1);
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


void InfoEngine::initBlockLabel5() {
  _blocks["label5"]->setText("text", RLI_STR_PP12p);
}

void InfoEngine::initBlockBand() {
  _blocks["band"]->setText("text", RLI_STR_S_BAND);
}

void InfoEngine::initBlockLabel1() {
  _blocks["label1"]->setText("text", RLI_STR_NORTH);
}

void InfoEngine::initBlockLabel2() {
  _blocks["label2"]->setText("text", RLI_STR_RM);
}

void InfoEngine::initBlockLabel3() {
  _blocks["label3"]->setText("text", RLI_STR_WATER);
}

void InfoEngine::initBlockLabel4() {
  _blocks["label4"]->setText("text", RLI_STR_LOD);
}



void InfoEngine::initBlockFps() {
  _blocks["fps"]->setText("label", RLI_STR_FPS);
  _blocks["fps"]->setText("value", QByteArray("0"));
}

void InfoEngine::setFps(int fps) {
  _blocks["fps"]->setText("value", QString::number(fps).toLocal8Bit());
}



void InfoEngine::initBlockScale() {
  _blocks["scale"]->setText("scale1", QByteArray("0.125"));
  _blocks["scale"]->setText("slash", QByteArray("/"));
  _blocks["scale"]->setText("scale2", QByteArray("0.025"));
  _blocks["scale"]->setText("units", RLI_STR_NM);
}

void InfoEngine::initBlockVn() {
  _blocks["vn"]->setText("header", RLI_STR_EBL);

  _blocks["vn"]->setText("table_0_0", RLI_STR_B);
  _blocks["vn"]->setText("table_0_1", QByteArray("0.00"));
  _blocks["vn"]->setText("table_0_2", RLI_STR_DEGREE_SIGN);

  _blocks["vn"]->setText("table_1_0", RLI_STR_CU);
  _blocks["vn"]->setText("table_1_1", QByteArray("0.00"));
  _blocks["vn"]->setText("table_1_2", RLI_STR_GRAD_RB);
}

void InfoEngine::initBlockVd() {
  _blocks["vd"]->setText("header", RLI_STR_VRM);

  _blocks["vd"]->setText("table_0_0", QByteArray("0.00"));
  _blocks["vd"]->setText("table_0_1", RLI_STR_NM);
}

void InfoEngine::initBlockCourse() {
  _blocks["course"]->setText("table_0_0", RLI_STR_GYRO_HDG);
  _blocks["course"]->setText("table_0_1", QByteArray("0"));
  _blocks["course"]->setText("table_0_2", RLI_STR_DEGREE_SIGN);

  _blocks["course"]->setText("table_1_0", RLI_STR_MAN_SPD);
  _blocks["course"]->setText("table_1_1", QByteArray("0"));
  _blocks["course"]->setText("table_1_2", RLI_STR_NM);
}


void InfoEngine::initBlockPosition() {
  _blocks["position"]->setText("table_0_0", RLI_STR_LAT);
  _blocks["position"]->setText("table_0_1", RLI_STR_BLANK);

  _blocks["position"]->setText("table_1_0", RLI_STR_LON);
  _blocks["position"]->setText("table_1_1", RLI_STR_BLANK);
}

void InfoEngine::onPositionChanged(const std::pair<float, float>& position) {
  _blocks["position"]->setText("table_0_1", QString::number(position.first, 'd', 2).toLocal8Bit());
  _blocks["position"]->setText("table_1_1", QString::number(position.second, 'd', 2).toLocal8Bit());
}


void InfoEngine::initBlockBlank() {

}


void InfoEngine::initBlockClock() {
  _blocks["clock"]->setText("label", RLI_STR_TIME);
  _blocks["clock"]->setText("time", QTime::currentTime().toString().toLocal8Bit());
}

void InfoEngine::secondChanged() {
  _blocks["clock"]->setText("time", QTime::currentTime().toString().toLocal8Bit());
}



void InfoEngine::initBlockDanger() {
  _blocks["danger"]->setText("label", RLI_STR_DANGER_TRG);
}

void InfoEngine::initBlockTails() {
  _blocks["tails"]->setText("table_0_0", RLI_STR_TAILS);
  _blocks["tails"]->setText("table_0_1", RLI_STR_OFF);
  _blocks["tails"]->setText("table_0_2", RLI_STR_MIN);
}

void InfoEngine::initBlockDetails() {
  _blocks["danger-details"]->setText("table_0_0", RLI_STR_CPA);
  _blocks["danger-details"]->setText("table_0_1", QByteArray("0"));
  _blocks["danger-details"]->setText("table_0_2", RLI_STR_NM);

  _blocks["danger-details"]->setText("table_1_0", RLI_STR_TCPA_LIMIT);
  _blocks["danger-details"]->setText("table_1_1", QByteArray("0"));
  _blocks["danger-details"]->setText("table_1_2", RLI_STR_MIN);
}

void InfoEngine::initBlockVector() {
  _blocks["vector"]->setText("table_0_0", RLI_STR_VECTOR);
  _blocks["vector"]->setText("table_0_1", QByteArray("20"));
  _blocks["vector"]->setText("table_0_2", RLI_STR_MIN);
}

void InfoEngine::initBlockTargets() {
  _blocks["targets"]->setText("header", RLI_STR_TRG_ALL);
  _blocks["targets"]->setText("current-target", QByteArray("0"));
  _blocks["targets"]->setText("target-count", QByteArray("0"));

  _blocks["targets"]->setText("table_0_0", RLI_STR_BEARING);
  _blocks["targets"]->setText("table_0_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_0_2", RLI_STR_DEGREE_SIGN);

  _blocks["targets"]->setText("table_1_0", RLI_STR_RANGE);
  _blocks["targets"]->setText("table_1_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_1_2", RLI_STR_NM);

  _blocks["targets"]->setText("table_2_0", RLI_STR_COURSE_W);
  _blocks["targets"]->setText("table_2_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_2_2", RLI_STR_DEGREE_SIGN);

  _blocks["targets"]->setText("table_3_0", RLI_STR_SPEED_W);
  _blocks["targets"]->setText("table_3_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_3_2", RLI_STR_KTS);

  _blocks["targets"]->setText("table_4_0", RLI_STR_CPA);
  _blocks["targets"]->setText("table_4_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_4_2", RLI_STR_NM);

  _blocks["targets"]->setText("table_5_0", RLI_STR_TCPA);
  _blocks["targets"]->setText("table_5_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_5_2", RLI_STR_MIN);

  _blocks["targets"]->setText("table_6_0", RLI_STR_DCC);
  _blocks["targets"]->setText("table_6_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_6_2", RLI_STR_NM);

  _blocks["targets"]->setText("table_7_0", RLI_STR_TCC);
  _blocks["targets"]->setText("table_7_1", QByteArray("0.00"));
  _blocks["targets"]->setText("table_7_2", RLI_STR_MIN);
}

void InfoEngine::onTargetCountChanged(int count) {
  _blocks["targets"]->setText("target-count", QString::number(count).toLocal8Bit());
}

void InfoEngine::onSelectedTargetUpdated(const QString& tag, const RadarTarget& trgt) {
  _blocks["targets"]->setText("current-target", tag.toLocal8Bit());

  _blocks["targets"]->setText("table_2_1", QString::number(trgt.CourseOverGround).left(6).toLatin1());
  _blocks["targets"]->setText("table_3_1", QString::number(trgt.SpeedOverGround).left(6).toLatin1());
}


void InfoEngine::initBlockCursor() {
  _blocks["cursor"]->setText("header", RLI_STR_CURSOR);

  _blocks["cursor"]->setText("table_0_0", RLI_STR_BEARING);
  _blocks["cursor"]->setText("table_0_1", QByteArray("0.00"));
  _blocks["cursor"]->setText("table_0_2", RLI_STR_DEGREE_SIGN);

  _blocks["cursor"]->setText("table_1_0", RLI_STR_RANGE);
  _blocks["cursor"]->setText("table_1_1", QByteArray("0.00"));
  _blocks["cursor"]->setText("table_1_2", RLI_STR_NM);
}
