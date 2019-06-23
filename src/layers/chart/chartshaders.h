#ifndef CHARTSHADERFACTORY_H
#define CHARTSHADERFACTORY_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

typedef enum CHART_SHADER_COMMON_UNIFORMS
{ COMMON_UNIFORMS_NORTH           = 0
, COMMON_UNIFORMS_CENTER          = 1
, COMMON_UNIFORMS_SCALE           = 2
, COMMON_UNIFORMS_PATTERN_TEX_ID  = 3
, COMMON_UNIFORMS_PATTERN_TEX_DIM = 4
, COMMON_UNIFORMS_MVP_MATRIX      = 5
, COMMON_UNIFORMS_DISPLAY_ORDER   = 6
, COMMON_UNIFORMS_COUNT           = 7
} CHART_SHADER_COMMON_UNIFORMS;



typedef enum CHART_SHADER_AREA_UNIFORMS
{ AREA_UNIFORMS_COLOR_TABLE_TEX   = COMMON_UNIFORMS_COUNT+0
, AREA_UNIFORMS_COUNT             = COMMON_UNIFORMS_COUNT+1
} CHART_SHADER_AREA_UNIFORMS;

typedef enum CHART_SHADER_AREA_ATTRIBUTES
{ AREA_ATTRIBUTES_COORDS          = 0
, AREA_ATTRIBUTES_COLOR_INDEX     = 1
, AREA_ATTRIBUTES_PATTERN_INDEX   = 2
, AREA_ATTRIBUTES_PATTERN_DIM     = 3
, AREA_ATTRIBUTES_COUNT           = 4
} CHART_SHADER_AREA_ATTRIBUTES;



typedef enum CHART_SHADER_LINE_UNIFORMS
{ LINE_UNIFORMS_COLOR_TABLE_TEX   = COMMON_UNIFORMS_COUNT+0
, LINE_UNIFORMS_COUNT             = COMMON_UNIFORMS_COUNT+1
} CHART_SHADER_LINE_UNIFORMS;

typedef enum CHART_SHADER_LINE_ATTRIBUTES
{ LINE_ATTRIBUTES_COORDS1         = 0
, LINE_ATTRIBUTES_COORDS2         = 1
, LINE_ATTRIBUTES_DISTANCE        = 2
, LINE_ATTRIBUTES_ORDER           = 3
, LINE_ATTRIBUTES_COLOR_INDEX     = 4
, LINE_ATTRIBUTES_PATTERN_INDEX   = 5
, LINE_ATTRIBUTES_PATTERN_DIM     = 6
, LINE_ATTRIBUTES_COUNT           = 7
} CHART_SHADER_LINE_ATTRIBUTES;



typedef enum CHART_SHADER_TEXT_UNIFORMS
{ TEXT_UNIFORMS_COUNT             = COMMON_UNIFORMS_COUNT+0
} CHART_SHADER_TEXT_UNIFORMS;

typedef enum CHART_SHADER_TEXT_ATTRIBUTES
{ TEXT_ATTRIBUTES_COORDS          = 0
, TEXT_ATTRIBUTES_POINT_ORDER     = 1
, TEXT_ATTRIBUTES_CHAR_SHIFT      = 2
, TEXT_ATTRIBUTES_CHAR_VALUE      = 3
, TEXT_ATTRIBUTES_COUNT           = 4
} CHART_SHADER_TEXT_ATTRIBUTES;



typedef enum CHART_SHADER_MARK_UNIFORMS
{ MARK_UNIFORMS_COUNT             = COMMON_UNIFORMS_COUNT+0
} CHART_SHADER_MARK_UNIFORMS;

typedef enum CHART_SHADER_MARK_ATTRIBUTES
{ MARK_ATTRIBUTES_WORLD_COORDS    = 0
, MARK_ATTRIBUTES_VERTEX_OFFSET   = 1
, MARK_ATTRIBUTES_TEX_COORDS      = 2
, MARK_ATTRIBUTES_COUNT           = 3
} CHART_SHADER_MARK_ATTRIBUTES;



class ChartShaders : protected QOpenGLFunctions {
public:
  ChartShaders(QOpenGLContext* context);
  ~ChartShaders();

  inline QOpenGLShaderProgram* getChartAreaProgram() { return chart_area_program; }
  inline QOpenGLShaderProgram* getChartLineProgram() { return chart_line_program; }
  inline QOpenGLShaderProgram* getChartTextProgram() { return chart_text_program; }
  inline QOpenGLShaderProgram* getChartMarkProgram() { return chart_mark_program; }

  inline int getAreaUniformLoc(unsigned int ind) const { return (ind < AREA_UNIFORMS_COUNT) ? area_unif_locs[ind] : 0; }
  inline int getLineUniformLoc(unsigned int ind) const { return (ind < LINE_UNIFORMS_COUNT) ? line_unif_locs[ind] : 0; }
  inline int getTextUniformLoc(unsigned int ind) const { return (ind < TEXT_UNIFORMS_COUNT) ? text_unif_locs[ind] : 0; }
  inline int getMarkUniformLoc(unsigned int ind) const { return (ind < MARK_UNIFORMS_COUNT) ? mark_unif_locs[ind] : 0; }

  inline int getAreaAttributeLoc(unsigned int ind) const { return (ind < AREA_ATTRIBUTES_COUNT) ? area_attr_locs[ind] : 0; }
  inline int getLineAttributeLoc(unsigned int ind) const { return (ind < LINE_ATTRIBUTES_COUNT) ? line_attr_locs[ind] : 0; }
  inline int getTextAttributeLoc(unsigned int ind) const { return (ind < TEXT_ATTRIBUTES_COUNT) ? text_attr_locs[ind] : 0; }
  inline int getMarkAttributeLoc(unsigned int ind) const { return (ind < MARK_ATTRIBUTES_COUNT) ? mark_attr_locs[ind] : 0; }

private:
  void initChartAreaProgram();
  void initChartLineProgram();
  void initChartTextProgram();
  void initChartMarkProgram();

  int area_unif_locs[AREA_UNIFORMS_COUNT];
  int area_attr_locs[AREA_ATTRIBUTES_COUNT];

  int line_unif_locs[LINE_UNIFORMS_COUNT];
  int line_attr_locs[LINE_ATTRIBUTES_COUNT];

  int text_unif_locs[TEXT_UNIFORMS_COUNT];
  int text_attr_locs[TEXT_ATTRIBUTES_COUNT];

  int mark_unif_locs[MARK_UNIFORMS_COUNT];
  int mark_attr_locs[MARK_ATTRIBUTES_COUNT];

  QOpenGLShaderProgram* chart_area_program;
  QOpenGLShaderProgram* chart_line_program;
  QOpenGLShaderProgram* chart_text_program;
  QOpenGLShaderProgram* chart_mark_program;
};

#endif // CHARTSHADERFACTORY_H
