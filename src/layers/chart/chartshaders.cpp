#include "chartshaders.h"
#include "../../common/properties.h"

ChartShaders::ChartShaders(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  initAreaProgram();
  initLineProgram();
  initMarkProgram();
  initTextProgram();
}

ChartShaders::~ChartShaders() {
  delete area_program;
  delete line_program;
  delete text_program;
  delete mark_program;
}



void ChartShaders::initAreaProgram() {
  area_program = new QOpenGLShaderProgram();

  area_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_area.vert.glsl");
  area_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_area.frag.glsl");
  area_program->link();
  area_program->bind();

  area_unif_locs[COMMON_UNIF_NORTH]            = area_program->uniformLocation("north");
  area_unif_locs[COMMON_UNIF_CENTER]           = area_program->uniformLocation("center");
  area_unif_locs[COMMON_UNIF_SCALE]            = area_program->uniformLocation("scale");
  area_unif_locs[COMMON_UNIF_PATTERN_TEX_ID]   = area_program->uniformLocation("pattern_tex");
  area_unif_locs[COMMON_UNIF_PATTERN_TEX_DIM]  = area_program->uniformLocation("assetdim");
  area_unif_locs[COMMON_UNIF_MVP_MATRIX]       = area_program->uniformLocation("mvp_matrix");
  area_unif_locs[COMMON_UNIF_DISPLAY_ORDER]    = area_program->uniformLocation("display_order");
  area_unif_locs[AREA_UNIF_COLOR_TABLE_TEX]    = area_program->uniformLocation("color_table_tex");

  area_attr_locs[AREA_ATTR_COORDS]         = area_program->attributeLocation("coords");
  area_attr_locs[AREA_ATTR_COLOR_INDEX]    = area_program->attributeLocation("color_index");
  area_attr_locs[AREA_ATTR_PATTERN_INDEX]  = area_program->attributeLocation("tex_origin");
  area_attr_locs[AREA_ATTR_PATTERN_DIM]    = area_program->attributeLocation("tex_dim");

  area_program->release();
}


void ChartShaders::initLineProgram() {
  line_program = new QOpenGLShaderProgram();

  line_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_line.vert.glsl");
  line_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_line.frag.glsl");
  line_program->link();
  line_program->bind();

  line_unif_locs[COMMON_UNIF_NORTH]            = line_program->uniformLocation("north");
  line_unif_locs[COMMON_UNIF_CENTER]           = line_program->uniformLocation("center");
  line_unif_locs[COMMON_UNIF_SCALE]            = line_program->uniformLocation("scale");
  line_unif_locs[COMMON_UNIF_PATTERN_TEX_ID]   = line_program->uniformLocation("pattern_tex");
  line_unif_locs[COMMON_UNIF_PATTERN_TEX_DIM]  = line_program->uniformLocation("assetdim");
  line_unif_locs[COMMON_UNIF_MVP_MATRIX]       = line_program->uniformLocation("mvp_matrix");
  line_unif_locs[COMMON_UNIF_DISPLAY_ORDER]    = line_program->uniformLocation("display_order");
  line_unif_locs[LINE_UNIF_COLOR_TABLE_TEX]    = line_program->uniformLocation("color_table_tex");

  line_attr_locs[LINE_ATTR_COORDS1]        = line_program->attributeLocation("coords1");
  line_attr_locs[LINE_ATTR_COORDS2]        = line_program->attributeLocation("coords2");
  line_attr_locs[LINE_ATTR_DISTANCE]       = line_program->attributeLocation("dist");
  line_attr_locs[LINE_ATTR_ORDER]          = line_program->attributeLocation("order");
  line_attr_locs[LINE_ATTR_COLOR_INDEX]    = line_program->attributeLocation("color_index");
  line_attr_locs[LINE_ATTR_PATTERN_INDEX]  = line_program->attributeLocation("tex_orig");
  line_attr_locs[LINE_ATTR_PATTERN_DIM]    = line_program->attributeLocation("tex_dim");

  line_program->release();
}

void ChartShaders::initTextProgram() {
  text_program = new QOpenGLShaderProgram();

  text_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_text.vert.glsl");
  text_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_text.frag.glsl");
  text_program->link();
  text_program->bind();

  text_unif_locs[COMMON_UNIF_NORTH]          = text_program->uniformLocation("north");
  text_unif_locs[COMMON_UNIF_CENTER]         = text_program->uniformLocation("center");
  text_unif_locs[COMMON_UNIF_SCALE]          = text_program->uniformLocation("scale");
  text_unif_locs[COMMON_UNIF_PATTERN_TEX_ID] = text_program->uniformLocation("glyph_tex");
  text_unif_locs[COMMON_UNIF_MVP_MATRIX]     = text_program->uniformLocation("mvp_matrix");
  text_unif_locs[COMMON_UNIF_DISPLAY_ORDER]  = text_program->uniformLocation("display_order");

  text_attr_locs[TEXT_ATTR_COORDS]         = text_program->attributeLocation("coords");
  text_attr_locs[TEXT_ATTR_POINT_ORDER]    = text_program->attributeLocation("point_order");
  text_attr_locs[TEXT_ATTR_CHAR_SHIFT]     = text_program->attributeLocation("char_shift");
  text_attr_locs[TEXT_ATTR_CHAR_VALUE]     = text_program->attributeLocation("char_val");

  text_program->release();
}

void ChartShaders::initMarkProgram() {
  mark_program = new QOpenGLShaderProgram();

  mark_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_mark.vert.glsl");
  mark_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_mark.frag.glsl");
  mark_program->link();
  mark_program->bind();

  mark_unif_locs[COMMON_UNIF_NORTH]            = mark_program->uniformLocation("north");
  mark_unif_locs[COMMON_UNIF_CENTER]           = mark_program->uniformLocation("center");
  mark_unif_locs[COMMON_UNIF_SCALE]            = mark_program->uniformLocation("scale");
  mark_unif_locs[COMMON_UNIF_PATTERN_TEX_ID]   = mark_program->uniformLocation("pattern_tex");
  mark_unif_locs[COMMON_UNIF_PATTERN_TEX_DIM]  = mark_program->uniformLocation("assetdim");
  mark_unif_locs[COMMON_UNIF_MVP_MATRIX]       = mark_program->uniformLocation("mvp_matrix");
  mark_unif_locs[COMMON_UNIF_DISPLAY_ORDER]    = mark_program->uniformLocation("display_order");

  mark_attr_locs[MARK_ATTR_WORLD_COORDS]   = mark_program->attributeLocation("coords");
  mark_attr_locs[MARK_ATTR_VERTEX_OFFSET]  = mark_program->attributeLocation("vertex_offset");
  mark_attr_locs[MARK_ATTR_TEX_COORDS]     = mark_program->attributeLocation("tex_coords");

  mark_program->release();
}
