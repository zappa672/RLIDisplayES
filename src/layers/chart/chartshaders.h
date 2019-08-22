#ifndef CHARTSHADERFACTORY_H
#define CHARTSHADERFACTORY_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

typedef enum CHART_SHADER_COMMON_UNIFORMS
{ COMMON_UNIF_NORTH           = 0
, COMMON_UNIF_CENTER          = 1
, COMMON_UNIF_SCALE           = 2
, COMMON_UNIF_PATTERN_TEX_ID  = 3
, COMMON_UNIF_PATTERN_TEX_DIM = 4
, COMMON_UNIF_MVP_MATRIX      = 5
, COMMON_UNIF_DISPLAY_ORDER   = 6
, COMMON_UNIF_COUNT           = 7
} CHART_SHADER_COMMON_UNIFORMS;



typedef enum CHART_SHADER_AREA_UNIFORMS
{ AREA_UNIF_COLOR_TABLE_TEX   = COMMON_UNIF_COUNT+0
, AREA_UNIF_COUNT             = COMMON_UNIF_COUNT+1
} CHART_SHADER_AREA_UNIFORMS;

typedef enum CHART_SHADER_AREA_ATTRIBUTES
{ AREA_ATTR_COORDS          = 0
, AREA_ATTR_COLOR_INDEX     = 1
, AREA_ATTR_PATTERN_INDEX   = 2
, AREA_ATTR_PATTERN_DIM     = 3
, AREA_ATTR_COUNT           = 4
} CHART_SHADER_AREA_ATTRIBUTES;



typedef enum CHART_SHADER_LINE_UNIFORMS
{ LINE_UNIF_COLOR_TABLE_TEX   = COMMON_UNIF_COUNT+0
, LINE_UNIF_COUNT             = COMMON_UNIF_COUNT+1
} CHART_SHADER_LINE_UNIFORMS;

typedef enum CHART_SHADER_LINE_ATTRIBUTES
{ LINE_ATTR_COORDS1         = 0
, LINE_ATTR_COORDS2         = 1
, LINE_ATTR_DISTANCE        = 2
, LINE_ATTR_ORDER           = 3
, LINE_ATTR_COLOR_INDEX     = 4
, LINE_ATTR_PATTERN_INDEX   = 5
, LINE_ATTR_PATTERN_DIM     = 6
, LINE_ATTR_COUNT           = 7
} CHART_SHADER_LINE_ATTRIBUTES;



typedef enum CHART_SHADER_TEXT_UNIFORMS
{ TEXT_UNIF_COUNT             = COMMON_UNIF_COUNT+0
} CHART_SHADER_TEXT_UNIFORMS;

typedef enum CHART_SHADER_TEXT_ATTRIBUTES
{ TEXT_ATTR_COORDS          = 0
, TEXT_ATTR_POINT_ORDER     = 1
, TEXT_ATTR_CHAR_SHIFT      = 2
, TEXT_ATTR_CHAR_VALUE      = 3
, TEXT_ATTR_COUNT           = 4
} CHART_SHADER_TEXT_ATTRIBUTES;



typedef enum CHART_SHADER_MARK_UNIFORMS
{ MARK_UNIF_COUNT             = COMMON_UNIF_COUNT+0
} CHART_SHADER_MARK_UNIFORMS;

typedef enum CHART_SHADER_MARK_ATTRIBUTES
{ MARK_ATTR_WORLD_COORDS    = 0
, MARK_ATTR_VERTEX_OFFSET   = 1
, MARK_ATTR_TEX_COORDS      = 2
, MARK_ATTR_COUNT           = 3
} CHART_SHADER_MARK_ATTRIBUTES;



class ChartShaders : protected QOpenGLFunctions {
public:
  ChartShaders(QOpenGLContext* context);
  ~ChartShaders();

  inline QOpenGLShaderProgram* getAreaProgram() { return area_program; }
  inline QOpenGLShaderProgram* getLineProgram() { return line_program; }
  inline QOpenGLShaderProgram* getTextProgram() { return text_program; }
  inline QOpenGLShaderProgram* getMarkProgram() { return mark_program; }

  inline int getAreaUnifLoc(unsigned int ind) const { return (ind < AREA_UNIF_COUNT) ? area_unif_locs[ind] : 0; }
  inline int getLineUnifLoc(unsigned int ind) const { return (ind < LINE_UNIF_COUNT) ? line_unif_locs[ind] : 0; }
  inline int getTextUnifLoc(unsigned int ind) const { return (ind < TEXT_UNIF_COUNT) ? text_unif_locs[ind] : 0; }
  inline int getMarkUnifLoc(unsigned int ind) const { return (ind < MARK_UNIF_COUNT) ? mark_unif_locs[ind] : 0; }

  inline int getAreaAttrLoc(unsigned int ind) const { return (ind < AREA_ATTR_COUNT) ? area_attr_locs[ind] : 0; }
  inline int getLineAttrLoc(unsigned int ind) const { return (ind < LINE_ATTR_COUNT) ? line_attr_locs[ind] : 0; }
  inline int getTextAttrLoc(unsigned int ind) const { return (ind < TEXT_ATTR_COUNT) ? text_attr_locs[ind] : 0; }
  inline int getMarkAttrLoc(unsigned int ind) const { return (ind < MARK_ATTR_COUNT) ? mark_attr_locs[ind] : 0; }

private:
  void initAreaProgram();
  void initLineProgram();
  void initTextProgram();
  void initMarkProgram();

  int area_unif_locs[AREA_UNIF_COUNT];
  int area_attr_locs[AREA_ATTR_COUNT];

  int line_unif_locs[LINE_UNIF_COUNT];
  int line_attr_locs[LINE_ATTR_COUNT];

  int text_unif_locs[TEXT_UNIF_COUNT];
  int text_attr_locs[TEXT_ATTR_COUNT];

  int mark_unif_locs[MARK_UNIF_COUNT];
  int mark_attr_locs[MARK_ATTR_COUNT];

  QOpenGLShaderProgram* area_program;
  QOpenGLShaderProgram* line_program;
  QOpenGLShaderProgram* text_program;
  QOpenGLShaderProgram* mark_program;
};

#endif // CHARTSHADERFACTORY_H
