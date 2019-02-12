#include "infoengine.h"

#include <QTime>

#include "../../common/properties.h"

static constexpr int RLI_PANEL_GAIN_TEXT_ID     = 0;
static constexpr int RLI_PANEL_WATER_TEXT_ID    = 0;
static constexpr int RLI_PANEL_RAIN_TEXT_ID     = 0;
static constexpr int RLI_PANEL_APCH_TEXT_ID     = 0;
static constexpr int RLI_PANEL_EMISSION_TEXT_ID = 0;


static constexpr int RLI_PANEL_LABEL5_TEXT_ID   = 0;
static constexpr int RLI_PANEL_BAND_TEXT_ID     = 0;
static constexpr int RLI_PANEL_ORIENTATION_TEXT_ID   = 0;
static constexpr int RLI_PANEL_LABEL2_TEXT_ID   = 0;
static constexpr int RLI_PANEL_LABEL3_TEXT_ID   = 0;
static constexpr int RLI_PANEL_LABEL4_TEXT_ID   = 0;

static constexpr int RLI_PANEL_FPS_LABEL_TEXT_ID  = 0;
static constexpr int RLI_PANEL_FPS_VALUE_TEXT_ID  = 1;


static constexpr int RLI_PANEL_SCALE_SCALE1_TEXT_ID = 0;
static constexpr int RLI_PANEL_SCALE_SLASH_TEXT_ID  = 1;
static constexpr int RLI_PANEL_SCALE_SCALE2_TEXT_ID = 2;
static constexpr int RLI_PANEL_SCALE_UNITS_TEXT_ID  = 3;

static constexpr int RLI_PANEL_VN_HEADER_TEXT_ID    = 0;
static constexpr int RLI_PANEL_VN_TBL_0_0_TEXT_ID   = 1;
static constexpr int RLI_PANEL_VN_TBL_0_1_TEXT_ID   = 2;
static constexpr int RLI_PANEL_VN_TBL_0_2_TEXT_ID   = 3;
static constexpr int RLI_PANEL_VN_TBL_1_0_TEXT_ID   = 4;
static constexpr int RLI_PANEL_VN_TBL_1_1_TEXT_ID   = 5;
static constexpr int RLI_PANEL_VN_TBL_1_2_TEXT_ID   = 6;

static constexpr int RLI_PANEL_VD_HEADER_TEXT_ID    = 0;
static constexpr int RLI_PANEL_VD_TBL_0_0_TEXT_ID   = 1;
static constexpr int RLI_PANEL_VD_TBL_0_1_TEXT_ID   = 2;

static constexpr int RLI_PANEL_COURSE_TBL_0_0_TEXT_ID = 0;
static constexpr int RLI_PANEL_COURSE_TBL_0_1_TEXT_ID = 1;
static constexpr int RLI_PANEL_COURSE_TBL_0_2_TEXT_ID = 2;
static constexpr int RLI_PANEL_COURSE_TBL_1_0_TEXT_ID = 3;
static constexpr int RLI_PANEL_COURSE_TBL_1_1_TEXT_ID = 4;
static constexpr int RLI_PANEL_COURSE_TBL_1_2_TEXT_ID = 5;


static constexpr int RLI_PANEL_POSITION_TBL_0_0_TEXT_ID = 0;
static constexpr int RLI_PANEL_POSITION_TBL_0_1_TEXT_ID = 1;
static constexpr int RLI_PANEL_POSITION_TBL_1_0_TEXT_ID = 2;
static constexpr int RLI_PANEL_POSITION_TBL_1_1_TEXT_ID = 3;

//static constexpr int RLI_PANEL_BLANK...

static constexpr int RLI_PANEL_CLOCK_LABEL_TEXT_ID  = 0;
static constexpr int RLI_PANEL_CLOCK_TIME_TEXT_ID   = 1;


static constexpr int RLI_PANEL_DANGER_LABEL_TEXT_ID = 0;

static constexpr int RLI_PANEL_TAILS_TBL_0_0_TEXT_ID  = 0;
static constexpr int RLI_PANEL_TAILS_TBL_0_1_TEXT_ID  = 1;
static constexpr int RLI_PANEL_TAILS_TBL_0_2_TEXT_ID  = 2;

static constexpr int RLI_PANEL_DANGER_DETAILS_TBL_0_0_TEXT_ID = 0;
static constexpr int RLI_PANEL_DANGER_DETAILS_TBL_0_1_TEXT_ID = 1;
static constexpr int RLI_PANEL_DANGER_DETAILS_TBL_0_2_TEXT_ID = 2;
static constexpr int RLI_PANEL_DANGER_DETAILS_TBL_1_0_TEXT_ID = 3;
static constexpr int RLI_PANEL_DANGER_DETAILS_TBL_1_1_TEXT_ID = 4;
static constexpr int RLI_PANEL_DANGER_DETAILS_TBL_1_2_TEXT_ID = 5;

static constexpr int RLI_PANEL_VECTOR_TBL_0_0_TEXT_ID = 0;
static constexpr int RLI_PANEL_VECTOR_TBL_0_1_TEXT_ID = 1;
static constexpr int RLI_PANEL_VECTOR_TBL_0_2_TEXT_ID = 2;

static constexpr int RLI_PANEL_TARGETS_HEADER_TEXT_ID   = 0;
static constexpr int RLI_PANEL_TARGETS_CURRENT_TEXT_ID  = 1;
static constexpr int RLI_PANEL_TARGETS_COUNT_TEXT_ID    = 2;
static constexpr int RLI_PANEL_TARGETS_TBL_0_0_TEXT_ID  = 3;
static constexpr int RLI_PANEL_TARGETS_TBL_0_1_TEXT_ID  = 4;
static constexpr int RLI_PANEL_TARGETS_TBL_0_2_TEXT_ID  = 5;
static constexpr int RLI_PANEL_TARGETS_TBL_1_0_TEXT_ID  = 6;
static constexpr int RLI_PANEL_TARGETS_TBL_1_1_TEXT_ID  = 7;
static constexpr int RLI_PANEL_TARGETS_TBL_1_2_TEXT_ID  = 8;
static constexpr int RLI_PANEL_TARGETS_TBL_2_0_TEXT_ID  = 9;
static constexpr int RLI_PANEL_TARGETS_TBL_2_1_TEXT_ID  = 10;
static constexpr int RLI_PANEL_TARGETS_TBL_2_2_TEXT_ID  = 11;
static constexpr int RLI_PANEL_TARGETS_TBL_3_0_TEXT_ID  = 12;
static constexpr int RLI_PANEL_TARGETS_TBL_3_1_TEXT_ID  = 13;
static constexpr int RLI_PANEL_TARGETS_TBL_3_2_TEXT_ID  = 14;
static constexpr int RLI_PANEL_TARGETS_TBL_4_0_TEXT_ID  = 15;
static constexpr int RLI_PANEL_TARGETS_TBL_4_1_TEXT_ID  = 16;
static constexpr int RLI_PANEL_TARGETS_TBL_4_2_TEXT_ID  = 17;
static constexpr int RLI_PANEL_TARGETS_TBL_5_0_TEXT_ID  = 18;
static constexpr int RLI_PANEL_TARGETS_TBL_5_1_TEXT_ID  = 19;
static constexpr int RLI_PANEL_TARGETS_TBL_5_2_TEXT_ID  = 20;
static constexpr int RLI_PANEL_TARGETS_TBL_6_0_TEXT_ID  = 21;
static constexpr int RLI_PANEL_TARGETS_TBL_6_1_TEXT_ID  = 22;
static constexpr int RLI_PANEL_TARGETS_TBL_6_2_TEXT_ID  = 23;
static constexpr int RLI_PANEL_TARGETS_TBL_7_0_TEXT_ID  = 24;
static constexpr int RLI_PANEL_TARGETS_TBL_7_1_TEXT_ID  = 25;
static constexpr int RLI_PANEL_TARGETS_TBL_7_2_TEXT_ID  = 26;

static constexpr int RLI_PANEL_CURSOR_HEADER_TEXT_ID  = 0;
static constexpr int RLI_PANEL_CURSOR_TBL_0_0_TEXT_ID = 1;
static constexpr int RLI_PANEL_CURSOR_TBL_0_1_TEXT_ID = 2;
static constexpr int RLI_PANEL_CURSOR_TBL_0_2_TEXT_ID = 3;
static constexpr int RLI_PANEL_CURSOR_TBL_1_0_TEXT_ID = 4;
static constexpr int RLI_PANEL_CURSOR_TBL_1_1_TEXT_ID = 5;
static constexpr int RLI_PANEL_CURSOR_TBL_1_2_TEXT_ID = 6;


const static std::map<int, std::map<QString, int>> panel_texts_map {
  {  InfoEngine::RLI_PANEL_GAIN,
    std::map<QString, int> { { "text", RLI_PANEL_GAIN_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_WATER,
    std::map<QString, int> { { "text", RLI_PANEL_WATER_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_RAIN,
    std::map<QString, int> { { "text", RLI_PANEL_RAIN_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_APCH,
    std::map<QString, int> { { "text", RLI_PANEL_APCH_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_EMISSION,
    std::map<QString, int> { { "text", RLI_PANEL_EMISSION_TEXT_ID } }
  }

, {  InfoEngine::RLI_PANEL_LABEL5,
    std::map<QString, int> { { "text", RLI_PANEL_LABEL5_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_BAND,
    std::map<QString, int> { { "text", RLI_PANEL_BAND_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_ORIENTATION,
    std::map<QString, int> { { "text", RLI_PANEL_ORIENTATION_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_LABEL2,
    std::map<QString, int> { { "text", RLI_PANEL_LABEL2_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_LABEL3,
    std::map<QString, int> { { "text", RLI_PANEL_LABEL3_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_LABEL4,
    std::map<QString, int> { { "text", RLI_PANEL_LABEL4_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_FPS,
    std::map<QString, int> { { "label", RLI_PANEL_FPS_LABEL_TEXT_ID }
                           , { "value", RLI_PANEL_FPS_VALUE_TEXT_ID } }
  }

, {  InfoEngine::RLI_PANEL_SCALE,
    std::map<QString, int> { { "scale1", RLI_PANEL_SCALE_SCALE1_TEXT_ID }
                           , { "slash" , RLI_PANEL_SCALE_SLASH_TEXT_ID  }
                           , { "scale2", RLI_PANEL_SCALE_SCALE2_TEXT_ID }
                           , { "units" , RLI_PANEL_SCALE_UNITS_TEXT_ID  } }
  }
, {  InfoEngine::RLI_PANEL_VN,
    std::map<QString, int> { { "header", RLI_PANEL_VN_HEADER_TEXT_ID }
                           , { "table_0_0", RLI_PANEL_VN_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_VN_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_VN_TBL_0_2_TEXT_ID }
                           , { "table_1_0", RLI_PANEL_VN_TBL_1_0_TEXT_ID }
                           , { "table_1_1", RLI_PANEL_VN_TBL_1_1_TEXT_ID }
                           , { "table_1_2", RLI_PANEL_VN_TBL_1_2_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_VD ,
    std::map<QString, int> { { "header", RLI_PANEL_VD_HEADER_TEXT_ID }
                           , { "table_0_0", RLI_PANEL_VD_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_VD_TBL_0_1_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_COURSE,
    std::map<QString, int> { { "table_0_0", RLI_PANEL_COURSE_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_COURSE_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_COURSE_TBL_0_2_TEXT_ID }
                           , { "table_1_0", RLI_PANEL_COURSE_TBL_1_0_TEXT_ID }
                           , { "table_1_1", RLI_PANEL_COURSE_TBL_1_1_TEXT_ID }
                           , { "table_1_2", RLI_PANEL_COURSE_TBL_1_2_TEXT_ID } }
  }

, {  InfoEngine::RLI_PANEL_POSITION,
    std::map<QString, int> { { "table_0_0", RLI_PANEL_POSITION_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_POSITION_TBL_0_1_TEXT_ID }
                           , { "table_1_0", RLI_PANEL_POSITION_TBL_1_0_TEXT_ID }
                           , { "table_1_1", RLI_PANEL_POSITION_TBL_1_1_TEXT_ID }}
  }
, {  InfoEngine::RLI_PANEL_BLANK,
    std::map<QString, int> { }
  }
, {  InfoEngine::RLI_PANEL_CLOCK,
    std::map<QString, int> { { "label", RLI_PANEL_CLOCK_LABEL_TEXT_ID }
                           , { "time" , RLI_PANEL_CLOCK_TIME_TEXT_ID  } }
  }

, {  InfoEngine::RLI_PANEL_DANGER,
    std::map<QString, int> { { "label", RLI_PANEL_DANGER_LABEL_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_TAILS,
    std::map<QString, int> { { "table_0_0", RLI_PANEL_TAILS_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_TAILS_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_TAILS_TBL_0_2_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_DANGER_DETAILS,
    std::map<QString, int> { { "table_0_0", RLI_PANEL_DANGER_DETAILS_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_DANGER_DETAILS_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_DANGER_DETAILS_TBL_0_2_TEXT_ID }
                           , { "table_1_0", RLI_PANEL_DANGER_DETAILS_TBL_1_0_TEXT_ID }
                           , { "table_1_1", RLI_PANEL_DANGER_DETAILS_TBL_1_1_TEXT_ID }
                           , { "table_1_2", RLI_PANEL_DANGER_DETAILS_TBL_1_2_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_VECTOR,
    std::map<QString, int> { { "table_0_0", RLI_PANEL_VECTOR_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_VECTOR_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_VECTOR_TBL_0_2_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_TARGETS,
    std::map<QString, int> { { "header"   , RLI_PANEL_TARGETS_HEADER_TEXT_ID  }
                           , { "current"  , RLI_PANEL_TARGETS_CURRENT_TEXT_ID }
                           , { "count"    , RLI_PANEL_TARGETS_COUNT_TEXT_ID   }
                           , { "table_0_0", RLI_PANEL_TARGETS_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_TARGETS_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_TARGETS_TBL_0_2_TEXT_ID }
                           , { "table_1_0", RLI_PANEL_TARGETS_TBL_1_0_TEXT_ID }
                           , { "table_1_1", RLI_PANEL_TARGETS_TBL_1_1_TEXT_ID }
                           , { "table_1_2", RLI_PANEL_TARGETS_TBL_1_2_TEXT_ID }
                           , { "table_2_0", RLI_PANEL_TARGETS_TBL_2_0_TEXT_ID }
                           , { "table_2_1", RLI_PANEL_TARGETS_TBL_2_1_TEXT_ID }
                           , { "table_2_2", RLI_PANEL_TARGETS_TBL_2_2_TEXT_ID }
                           , { "table_3_0", RLI_PANEL_TARGETS_TBL_3_0_TEXT_ID }
                           , { "table_3_1", RLI_PANEL_TARGETS_TBL_3_1_TEXT_ID }
                           , { "table_3_2", RLI_PANEL_TARGETS_TBL_3_2_TEXT_ID }
                           , { "table_4_0", RLI_PANEL_TARGETS_TBL_4_0_TEXT_ID }
                           , { "table_4_1", RLI_PANEL_TARGETS_TBL_4_1_TEXT_ID }
                           , { "table_4_2", RLI_PANEL_TARGETS_TBL_4_2_TEXT_ID }
                           , { "table_5_0", RLI_PANEL_TARGETS_TBL_5_0_TEXT_ID }
                           , { "table_5_1", RLI_PANEL_TARGETS_TBL_5_1_TEXT_ID }
                           , { "table_5_2", RLI_PANEL_TARGETS_TBL_5_2_TEXT_ID }
                           , { "table_6_0", RLI_PANEL_TARGETS_TBL_6_0_TEXT_ID }
                           , { "table_6_1", RLI_PANEL_TARGETS_TBL_6_1_TEXT_ID }
                           , { "table_6_2", RLI_PANEL_TARGETS_TBL_6_2_TEXT_ID }
                           , { "table_7_0", RLI_PANEL_TARGETS_TBL_7_0_TEXT_ID }
                           , { "table_7_1", RLI_PANEL_TARGETS_TBL_7_1_TEXT_ID }
                           , { "table_7_2", RLI_PANEL_TARGETS_TBL_7_2_TEXT_ID } }
  }
, {  InfoEngine::RLI_PANEL_CURSOR,
    std::map<QString, int> { { "header"   , RLI_PANEL_CURSOR_HEADER_TEXT_ID }
                           , { "table_0_0", RLI_PANEL_CURSOR_TBL_0_0_TEXT_ID }
                           , { "table_0_1", RLI_PANEL_CURSOR_TBL_0_1_TEXT_ID }
                           , { "table_0_2", RLI_PANEL_CURSOR_TBL_0_2_TEXT_ID }
                           , { "table_1_0", RLI_PANEL_CURSOR_TBL_1_0_TEXT_ID }
                           , { "table_1_1", RLI_PANEL_CURSOR_TBL_1_1_TEXT_ID }
                           , { "table_1_2", RLI_PANEL_CURSOR_TBL_1_2_TEXT_ID } }
  }
};




const static std::map<QString, int> panels_map {
    { "gain",     InfoEngine::RLI_PANEL_GAIN }
,   { "water",    InfoEngine::RLI_PANEL_WATER }
,   { "rain",     InfoEngine::RLI_PANEL_RAIN }
,   { "apch",     InfoEngine::RLI_PANEL_APCH }
,   { "emission", InfoEngine::RLI_PANEL_EMISSION }

,   { "label5",       InfoEngine::RLI_PANEL_LABEL5 }
,   { "band",         InfoEngine::RLI_PANEL_BAND }
,   { "orientation",  InfoEngine::RLI_PANEL_ORIENTATION }
,   { "label2",       InfoEngine::RLI_PANEL_LABEL2 }
,   { "label3",       InfoEngine::RLI_PANEL_LABEL3 }
,   { "label4",       InfoEngine::RLI_PANEL_LABEL4 }
,   { "fps",          InfoEngine::RLI_PANEL_FPS }

,   { "scale",    InfoEngine::RLI_PANEL_SCALE }
,   { "vn",       InfoEngine::RLI_PANEL_VN }
,   { "vd",       InfoEngine::RLI_PANEL_VD }
,   { "course",   InfoEngine::RLI_PANEL_COURSE }

,   { "position", InfoEngine::RLI_PANEL_POSITION }
,   { "blank",    InfoEngine::RLI_PANEL_BLANK }
,   { "clock",    InfoEngine::RLI_PANEL_CLOCK }

,   { "danger",   InfoEngine::RLI_PANEL_DANGER }
,   { "tails",    InfoEngine::RLI_PANEL_TAILS }
,   { "danger-details", InfoEngine::RLI_PANEL_DANGER_DETAILS }
,   { "vector",   InfoEngine::RLI_PANEL_VECTOR }
,   { "targets",  InfoEngine::RLI_PANEL_TARGETS }
,   { "cursor",   InfoEngine::RLI_PANEL_CURSOR }
};


InfoEngine::InfoEngine(RLILayout* layout, QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context)  {
  initializeOpenGLFunctions();

  for (const QString& name : layout->panels.keys()) {
    int panel_id = panels_map.at(name);
    _blocks[panel_id] = new InfoBlock(layout->panels[name], panel_texts_map.at(panel_id), context);
  }

  _prog = new QOpenGLShaderProgram(this);
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
  for (const QString& name : layout->panels.keys()) {
    int panel_id = panels_map.at(name);
    _blocks[panel_id]->resize(layout->panels[name], panel_texts_map.at(panel_id));
  }

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

  for (InfoBlock* block: _blocks) {
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
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, ord.size()*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, chars.size()*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
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
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
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
  initBlockOrientation();
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
  _blocks[RLI_PANEL_GAIN]->setText(RLI_PANEL_GAIN_TEXT_ID, RLI_STR_GAIN);
}

void InfoEngine::initBlockWater() {
  _blocks[RLI_PANEL_WATER]->setText(RLI_PANEL_WATER_TEXT_ID, RLI_STR_WATER);
}

void InfoEngine::initBlockRain() {
  _blocks[RLI_PANEL_RAIN]->setText(RLI_PANEL_RAIN_TEXT_ID, RLI_STR_RAIN);
}

void InfoEngine::initBlockApch() {
  _blocks[RLI_PANEL_APCH]->setText(RLI_PANEL_APCH_TEXT_ID, RLI_STR_AFC);
}

void InfoEngine::initBlockEmission() {
  _blocks[RLI_PANEL_EMISSION]->setText(RLI_PANEL_EMISSION_TEXT_ID, RLI_STR_EMISSION);
}

void InfoEngine::updateGain(float gain) {
  updateValueBar(RLI_PANEL_GAIN, gain);
}

void InfoEngine::updateWater(float water) {
  updateValueBar(RLI_PANEL_WATER, water);
}

void InfoEngine::updateRain(float rain) {
  updateValueBar(RLI_PANEL_RAIN, rain);
}

void InfoEngine::updateApch(float apch) {
  updateValueBar(RLI_PANEL_APCH, apch);
}

void InfoEngine::updateEmission(float emission) {
  updateValueBar(RLI_PANEL_EMISSION, emission);
}

void InfoEngine::updateValueBar(RLIPanel id, float value) {
  int max_bar_width = _blocks[id]->geometry().width() - _blocks[id]->rectangles()["splitter"].geometry.x() - 2;
  int bar_width = static_cast<int>((value / 255.f) * max_bar_width);

  QRect bar = _blocks[id]->rectangles()["bar"].geometry;

  bar.setWidth(bar_width);
  _blocks[id]->setRect("bar", bar);
}



void InfoEngine::initBlockLabel5() {
  _blocks[RLI_PANEL_LABEL5]->setText(RLI_PANEL_LABEL5_TEXT_ID, RLI_STR_PP12p);
}

void InfoEngine::initBlockBand() {
  _blocks[RLI_PANEL_BAND]->setText(RLI_PANEL_BAND_TEXT_ID, RLI_STR_S_BAND);
}


void InfoEngine::initBlockOrientation() {
  _blocks[RLI_PANEL_ORIENTATION]->setText(RLI_PANEL_ORIENTATION_TEXT_ID, RLI_STR_NORTH);
}

void InfoEngine::initBlockVn() {
  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_HEADER_TEXT_ID, RLI_STR_EBL);

  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_0_TEXT_ID, RLI_STR_B);
  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_2_TEXT_ID, RLI_STR_DEGREE_SIGN);

  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_1_0_TEXT_ID, RLI_STR_CU);
  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_1_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_1_2_TEXT_ID, RLI_STR_GRAD_RB);
}

void InfoEngine::initBlockCursor() {
  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_HEADER_TEXT_ID, RLI_STR_CURSOR);

  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_0_0_TEXT_ID, RLI_STR_BEARING);
  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_0_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_0_2_TEXT_ID, RLI_STR_DEGREE_SIGN);

  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_1_0_TEXT_ID, RLI_STR_RANGE);
  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_1_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_1_2_TEXT_ID, RLI_STR_NM);
}

void InfoEngine::onOrientationChanged(RLIOrientation orient) {
  switch (orient) {
    case RLIOrientation::RLIORIENT_HEAD:
      _blocks[RLI_PANEL_ORIENTATION]->setText(RLI_PANEL_ORIENTATION_TEXT_ID, RLI_STR_HEAD);
      break;
    case RLIOrientation::RLIORIENT_NORTH:
      _blocks[RLI_PANEL_ORIENTATION]->setText(RLI_PANEL_ORIENTATION_TEXT_ID, RLI_STR_NORTH);
      break;
    case RLIOrientation::RLIORIENT_COURSE:
      _blocks[RLI_PANEL_ORIENTATION]->setText(RLI_PANEL_ORIENTATION_TEXT_ID, RLI_STR_COURSE);
      break;
  }

  if (orient == RLIOrientation::RLIORIENT_NORTH) {
    _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_0_TEXT_ID, RLI_STR_B);
    _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_1_TEXT_ID, QByteArray("0.00"));
    _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_2_TEXT_ID, RLI_STR_DEGREE_SIGN);

    _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_0_0_TEXT_ID, RLI_STR_BEARING);
  } else {
    _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_0_TEXT_ID, QByteArray(""));
    _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_1_TEXT_ID, QByteArray(""));
    _blocks[RLI_PANEL_VN]->setText(RLI_PANEL_VN_TBL_0_2_TEXT_ID, QByteArray(""));

    _blocks[RLI_PANEL_CURSOR]->setText(RLI_PANEL_CURSOR_TBL_0_0_TEXT_ID, RLI_STR_HB);
  }
}



void InfoEngine::initBlockLabel2() {
  _blocks[RLI_PANEL_LABEL2]->setText(RLI_PANEL_LABEL2_TEXT_ID, RLI_STR_RM);
}

void InfoEngine::initBlockLabel3() {
  _blocks[RLI_PANEL_LABEL3]->setText(RLI_PANEL_LABEL3_TEXT_ID, RLI_STR_WATER);
}

void InfoEngine::initBlockLabel4() {
  _blocks[RLI_PANEL_LABEL4]->setText(RLI_PANEL_LABEL4_TEXT_ID, RLI_STR_LOD);
}



void InfoEngine::initBlockFps() {
  _blocks[RLI_PANEL_FPS]->setText(RLI_PANEL_FPS_LABEL_TEXT_ID, RLI_STR_FPS);
  _blocks[RLI_PANEL_FPS]->setText(RLI_PANEL_FPS_VALUE_TEXT_ID, QByteArray("0"));
}

void InfoEngine::setFps(int fps) {
  _blocks[RLI_PANEL_FPS]->setText(RLI_PANEL_FPS_VALUE_TEXT_ID, QString::number(fps).toLocal8Bit());
}



void InfoEngine::initBlockScale() {
  _blocks[RLI_PANEL_SCALE]->setText(RLI_PANEL_SCALE_SCALE1_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_SCALE]->setText(RLI_PANEL_SCALE_SLASH_TEXT_ID, QByteArray("/"));
  _blocks[RLI_PANEL_SCALE]->setText(RLI_PANEL_SCALE_SCALE2_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_SCALE]->setText(RLI_PANEL_SCALE_UNITS_TEXT_ID, RLI_STR_NM);
}

void InfoEngine::onScaleChanged(const rli_scale_t* scale) {
  _blocks[RLI_PANEL_SCALE]->setText(RLI_PANEL_SCALE_SCALE1_TEXT_ID, QByteArray(scale->display));
  _blocks[RLI_PANEL_SCALE]->setText(RLI_PANEL_SCALE_SCALE2_TEXT_ID, QByteArray(scale->dist_rng_display));
}



void InfoEngine::initBlockVd() {
  _blocks[RLI_PANEL_VD]->setText(RLI_PANEL_VD_HEADER_TEXT_ID, RLI_STR_VRM);

  _blocks[RLI_PANEL_VD]->setText(RLI_PANEL_VD_TBL_0_0_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_VD]->setText(RLI_PANEL_VD_TBL_0_1_TEXT_ID, RLI_STR_NM);
}



void InfoEngine::initBlockCourse() {
  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_0_0_TEXT_ID, RLI_STR_GYRO_HDG);
  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_0_1_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_0_2_TEXT_ID, RLI_STR_DEGREE_SIGN);

  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_1_0_TEXT_ID, RLI_STR_MAN_SPD);
  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_1_1_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_1_2_TEXT_ID, RLI_STR_NM);
}

void InfoEngine::onCourseChanged(double course) {
  _blocks[RLI_PANEL_COURSE]->setText(RLI_PANEL_COURSE_TBL_0_1_TEXT_ID, QString::number(course, 'd', 2).left(5).toLocal8Bit());
}


void InfoEngine::initBlockPosition() {
  _blocks[RLI_PANEL_POSITION]->setText(RLI_PANEL_POSITION_TBL_0_0_TEXT_ID, RLI_STR_LAT);
  _blocks[RLI_PANEL_POSITION]->setText(RLI_PANEL_POSITION_TBL_0_1_TEXT_ID, RLI_STR_BLANK);

  _blocks[RLI_PANEL_POSITION]->setText(RLI_PANEL_POSITION_TBL_1_0_TEXT_ID, RLI_STR_LON);
  _blocks[RLI_PANEL_POSITION]->setText(RLI_PANEL_POSITION_TBL_1_1_TEXT_ID, RLI_STR_BLANK);
}

void InfoEngine::onPositionChanged(const GeoPos& position) {
  _blocks[RLI_PANEL_POSITION]->setText(RLI_PANEL_POSITION_TBL_0_1_TEXT_ID, QString::number(position.lat, 'd', 2).toLocal8Bit());
  _blocks[RLI_PANEL_POSITION]->setText(RLI_PANEL_POSITION_TBL_1_1_TEXT_ID, QString::number(position.lon, 'd', 2).toLocal8Bit());
}




void InfoEngine::initBlockBlank() {

}


void InfoEngine::initBlockClock() {
  _blocks[RLI_PANEL_CLOCK]->setText(RLI_PANEL_CLOCK_LABEL_TEXT_ID, RLI_STR_TIME);
  _blocks[RLI_PANEL_CLOCK]->setText(RLI_PANEL_CLOCK_TIME_TEXT_ID, QTime::currentTime().toString().toLocal8Bit());
}

void InfoEngine::secondChanged() {
  _blocks[RLI_PANEL_CLOCK]->setText(RLI_PANEL_CLOCK_TIME_TEXT_ID, QTime::currentTime().toString().toLocal8Bit());
}



void InfoEngine::initBlockDanger() {
  _blocks[RLI_PANEL_DANGER]->setText(RLI_PANEL_DANGER_LABEL_TEXT_ID, RLI_STR_DANGER_TRG);
}

void InfoEngine::initBlockTails() {
  _blocks[RLI_PANEL_TAILS]->setText(RLI_PANEL_TAILS_TBL_0_0_TEXT_ID, RLI_STR_TAILS);
  _blocks[RLI_PANEL_TAILS]->setText(RLI_PANEL_TAILS_TBL_0_1_TEXT_ID, RLI_STR_OFF);
  _blocks[RLI_PANEL_TAILS]->setText(RLI_PANEL_TAILS_TBL_0_2_TEXT_ID, RLI_STR_MIN);
}

void InfoEngine::initBlockDetails() {
  _blocks[RLI_PANEL_DANGER_DETAILS]->setText(RLI_PANEL_DANGER_DETAILS_TBL_0_0_TEXT_ID, RLI_STR_CPA);
  _blocks[RLI_PANEL_DANGER_DETAILS]->setText(RLI_PANEL_DANGER_DETAILS_TBL_0_1_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_DANGER_DETAILS]->setText(RLI_PANEL_DANGER_DETAILS_TBL_0_2_TEXT_ID, RLI_STR_NM);

  _blocks[RLI_PANEL_DANGER_DETAILS]->setText(RLI_PANEL_DANGER_DETAILS_TBL_1_0_TEXT_ID, RLI_STR_TCPA_LIMIT);
  _blocks[RLI_PANEL_DANGER_DETAILS]->setText(RLI_PANEL_DANGER_DETAILS_TBL_1_1_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_DANGER_DETAILS]->setText(RLI_PANEL_DANGER_DETAILS_TBL_1_2_TEXT_ID, RLI_STR_MIN);
}

void InfoEngine::initBlockVector() {
  _blocks[RLI_PANEL_VECTOR]->setText(RLI_PANEL_VECTOR_TBL_0_0_TEXT_ID, RLI_STR_VECTOR);
  _blocks[RLI_PANEL_VECTOR]->setText(RLI_PANEL_VECTOR_TBL_0_1_TEXT_ID, QByteArray("20"));
  _blocks[RLI_PANEL_VECTOR]->setText(RLI_PANEL_VECTOR_TBL_0_2_TEXT_ID, RLI_STR_MIN);
}

void InfoEngine::initBlockTargets() {
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_HEADER_TEXT_ID, RLI_STR_TRG_ALL);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_CURRENT_TEXT_ID, QByteArray("0"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_COUNT_TEXT_ID, QByteArray("0"));

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_0_0_TEXT_ID, RLI_STR_BEARING);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_0_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_0_2_TEXT_ID, RLI_STR_DEGREE_SIGN);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_1_0_TEXT_ID, RLI_STR_RANGE);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_1_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_1_2_TEXT_ID, RLI_STR_NM);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_2_0_TEXT_ID, RLI_STR_COURSE_W);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_2_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_2_2_TEXT_ID, RLI_STR_DEGREE_SIGN);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_3_0_TEXT_ID, RLI_STR_SPEED_W);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_3_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_3_2_TEXT_ID, RLI_STR_KTS);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_4_0_TEXT_ID, RLI_STR_CPA);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_4_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_4_2_TEXT_ID, RLI_STR_NM);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_5_0_TEXT_ID, RLI_STR_TCPA);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_5_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_5_2_TEXT_ID, RLI_STR_MIN);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_6_0_TEXT_ID, RLI_STR_DCC);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_6_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_6_2_TEXT_ID, RLI_STR_NM);

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_7_0_TEXT_ID, RLI_STR_TCC);
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_7_1_TEXT_ID, QByteArray("0.00"));
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_7_2_TEXT_ID, RLI_STR_MIN);
}

void InfoEngine::onTargetCountChanged(int count) {
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_COUNT_TEXT_ID, QString::number(count).toLocal8Bit());
}

void InfoEngine::onSelectedTargetUpdated(const QString& tag, const RLITarget& trgt) {
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_CURRENT_TEXT_ID, tag.toLocal8Bit());

  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_2_1_TEXT_ID, QString::number(trgt.course_grnd).left(6).toLatin1());
  _blocks[RLI_PANEL_TARGETS]->setText(RLI_PANEL_TARGETS_TBL_3_1_TEXT_ID, QString::number(trgt.speed_grnd).left(6).toLatin1());
}
