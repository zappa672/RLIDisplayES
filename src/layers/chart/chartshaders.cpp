#include "chartshaders.h"
#include "../../common/properties.h"

ChartShaders::ChartShaders(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  initChartAreaProgram();
  initChartLineProgram();
  initChartMarkProgram();
  initChartTextProgram();
}

ChartShaders::~ChartShaders() {
  delete chart_area_program;
  delete chart_line_program;
  delete chart_text_program;
  delete chart_mark_program;
}




GLuint ChartShaders::getAreaUniformLoc(unsigned int index) {
  return (index < AREA_UNIFORMS_COUNT) ? area_unif_locs[index] : 0;
}

GLuint ChartShaders::getAreaAttributeLoc(unsigned int index) {
  return (index < AREA_ATTRIBUTES_COUNT) ? area_attr_locs[index]: 0;
}

GLuint ChartShaders::getLineUniformLoc(unsigned int index) {
  return (index < LINE_UNIFORMS_COUNT) ? line_unif_locs[index] : 0;
}

GLuint ChartShaders::getLineAttributeLoc(unsigned int index) {
  return (index < LINE_ATTRIBUTES_COUNT) ? line_attr_locs[index]: 0;
}

GLuint ChartShaders::getTextUniformLoc(unsigned int index) {
  return (index < TEXT_UNIFORMS_COUNT) ? text_unif_locs[index] : 0;
}

GLuint ChartShaders::getTextAttributeLoc(unsigned int index) {
  return (index < TEXT_ATTRIBUTES_COUNT) ? text_attr_locs[index] : 0;
}

GLuint ChartShaders::getMarkUniformLoc(unsigned int index) {
  return (index < MARK_UNIFORMS_COUNT) ? mark_unif_locs[index] : 0;
}

GLuint ChartShaders::getMarkAttributeLoc(unsigned int index) {
  return (index < MARK_ATTRIBUTES_COUNT) ? mark_attr_locs[index] : 0;
}



void ChartShaders::initChartAreaProgram() {
  chart_area_program = new QOpenGLShaderProgram();

  chart_area_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_area.vert.glsl");
  chart_area_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_area.frag.glsl");
  chart_area_program->link();
  chart_area_program->bind();

  area_unif_locs[COMMON_UNIFORMS_NORTH]            = chart_area_program->uniformLocation("north");
  area_unif_locs[COMMON_UNIFORMS_CENTER]           = chart_area_program->uniformLocation("center");
  area_unif_locs[COMMON_UNIFORMS_SCALE]            = chart_area_program->uniformLocation("scale");
  area_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_ID]   = chart_area_program->uniformLocation("pattern_tex");
  area_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_DIM]  = chart_area_program->uniformLocation("assetdim");
  area_unif_locs[COMMON_UNIFORMS_MVP_MATRIX]       = chart_area_program->uniformLocation("mvp_matrix");
  area_unif_locs[COMMON_UNIFORMS_DISPLAY_ORDER]    = chart_area_program->uniformLocation("display_order");
  area_unif_locs[AREA_UNIFORMS_COLOR_TABLE_TEX]    = chart_area_program->uniformLocation("color_table_tex");

  area_attr_locs[AREA_ATTRIBUTES_COORDS]         = chart_area_program->attributeLocation("coords");
  area_attr_locs[AREA_ATTRIBUTES_COLOR_INDEX]    = chart_area_program->attributeLocation("color_index");
  area_attr_locs[AREA_ATTRIBUTES_PATTERN_INDEX]  = chart_area_program->attributeLocation("tex_origin");
  area_attr_locs[AREA_ATTRIBUTES_PATTERN_DIM]    = chart_area_program->attributeLocation("tex_dim");

  chart_area_program->release();
}


void ChartShaders::initChartLineProgram() {
  chart_line_program = new QOpenGLShaderProgram();

  chart_line_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_line.vert.glsl");
  chart_line_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_line.frag.glsl");
  chart_line_program->link();
  chart_line_program->bind();

  line_unif_locs[COMMON_UNIFORMS_NORTH]            = chart_line_program->uniformLocation("north");
  line_unif_locs[COMMON_UNIFORMS_CENTER]           = chart_line_program->uniformLocation("center");
  line_unif_locs[COMMON_UNIFORMS_SCALE]            = chart_line_program->uniformLocation("scale");
  line_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_ID]   = chart_line_program->uniformLocation("pattern_tex");
  line_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_DIM]  = chart_line_program->uniformLocation("assetdim");
  line_unif_locs[COMMON_UNIFORMS_MVP_MATRIX]       = chart_line_program->uniformLocation("mvp_matrix");
  line_unif_locs[COMMON_UNIFORMS_DISPLAY_ORDER]    = chart_line_program->uniformLocation("display_order");
  line_unif_locs[LINE_UNIFORMS_COLOR_TABLE_TEX]    = chart_line_program->uniformLocation("color_table_tex");

  line_attr_locs[LINE_ATTRIBUTES_COORDS1]        = chart_line_program->attributeLocation("coords1");
  line_attr_locs[LINE_ATTRIBUTES_COORDS2]        = chart_line_program->attributeLocation("coords2");
  line_attr_locs[LINE_ATTRIBUTES_DISTANCE]       = chart_line_program->attributeLocation("dist");
  line_attr_locs[LINE_ATTRIBUTES_ORDER]          = chart_line_program->attributeLocation("order");
  line_attr_locs[LINE_ATTRIBUTES_COLOR_INDEX]    = chart_line_program->attributeLocation("color_index");
  line_attr_locs[LINE_ATTRIBUTES_PATTERN_INDEX]  = chart_line_program->attributeLocation("tex_orig");
  line_attr_locs[LINE_ATTRIBUTES_PATTERN_DIM]    = chart_line_program->attributeLocation("tex_dim");

  chart_line_program->release();
}

void ChartShaders::initChartTextProgram() {
  chart_text_program = new QOpenGLShaderProgram();

  chart_text_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_text.vert.glsl");
  chart_text_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_text.frag.glsl");
  chart_text_program->link();
  chart_text_program->bind();

  text_unif_locs[COMMON_UNIFORMS_NORTH]          = chart_text_program->uniformLocation("north");
  text_unif_locs[COMMON_UNIFORMS_CENTER]         = chart_text_program->uniformLocation("center");
  text_unif_locs[COMMON_UNIFORMS_SCALE]          = chart_text_program->uniformLocation("scale");
  text_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_ID] = chart_text_program->uniformLocation("glyph_tex");
  text_unif_locs[COMMON_UNIFORMS_MVP_MATRIX]     = chart_text_program->uniformLocation("mvp_matrix");
  text_unif_locs[COMMON_UNIFORMS_DISPLAY_ORDER]  = chart_text_program->uniformLocation("display_order");

  text_attr_locs[TEXT_ATTRIBUTES_COORDS]         = chart_text_program->attributeLocation("coords");
  text_attr_locs[TEXT_ATTRIBUTES_POINT_ORDER]    = chart_text_program->attributeLocation("point_order");
  text_attr_locs[TEXT_ATTRIBUTES_CHAR_SHIFT]     = chart_text_program->attributeLocation("char_shift");
  text_attr_locs[TEXT_ATTRIBUTES_CHAR_VALUE]     = chart_text_program->attributeLocation("char_val");

  chart_text_program->release();
}

void ChartShaders::initChartMarkProgram() {
  chart_mark_program = new QOpenGLShaderProgram();

  chart_mark_program->addShaderFromSourceFile(QOpenGLShader::Vertex, SHADERS_PATH + "chart_mark.vert.glsl");
  chart_mark_program->addShaderFromSourceFile(QOpenGLShader::Fragment, SHADERS_PATH + "chart_mark.frag.glsl");
  chart_mark_program->link();
  chart_mark_program->bind();

  mark_unif_locs[COMMON_UNIFORMS_NORTH]            = chart_mark_program->uniformLocation("north");
  mark_unif_locs[COMMON_UNIFORMS_CENTER]           = chart_mark_program->uniformLocation("center");
  mark_unif_locs[COMMON_UNIFORMS_SCALE]            = chart_mark_program->uniformLocation("scale");
  mark_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_ID]   = chart_mark_program->uniformLocation("pattern_tex");
  mark_unif_locs[COMMON_UNIFORMS_PATTERN_TEX_DIM]  = chart_mark_program->uniformLocation("assetdim");
  mark_unif_locs[COMMON_UNIFORMS_MVP_MATRIX]       = chart_mark_program->uniformLocation("mvp_matrix");
  mark_unif_locs[COMMON_UNIFORMS_DISPLAY_ORDER]    = chart_mark_program->uniformLocation("display_order");

  mark_attr_locs[MARK_ATTRIBUTES_WORLD_COORDS]   = chart_mark_program->attributeLocation("coords");
  mark_attr_locs[MARK_ATTRIBUTES_VERTEX_OFFSET]  = chart_mark_program->attributeLocation("vertex_offset");
  mark_attr_locs[MARK_ATTRIBUTES_TEX_COORDS]     = chart_mark_program->attributeLocation("tex_coords");

  chart_mark_program->release();
}
