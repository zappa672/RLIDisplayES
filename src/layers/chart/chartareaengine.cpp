#include "chartareaengine.h"

ChartAreaEngine::ChartAreaEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _point_count = 0;

  _is_color_uniform = false;
  _is_pattern_uniform = false;

  glGenBuffers(AREA_ATTRIBUTES_COUNT, _vbo_ids);
}

ChartAreaEngine::~ChartAreaEngine() {
  glDeleteBuffers(AREA_ATTRIBUTES_COUNT, _vbo_ids);
}

void ChartAreaEngine::clearData() {
  _point_count = 0;

  for (int i = 0; i < AREA_ATTRIBUTES_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChartAreaEngine::setData(S52AreaLayer* layer, S52Assets* assets, S52References* ref, int display_order) {
  _display_order = display_order;

  std::vector<GLfloat> color_inds;
  std::vector<GLfloat> tex_inds;
  std::vector<GLfloat> tex_dims;

  _is_pattern_uniform = layer->is_pattern_uniform;
  _patternLocation = assets->getAreaPatternLocation(ref->getColorScheme(), layer->pattern_ref);
  _patternSize = assets->getAreaPatternSize(ref->getColorScheme(), layer->pattern_ref);

  _is_color_uniform = layer->is_color_uniform;
  _color_ind = layer->color_ind;

  if ((!_is_color_uniform) || (!_is_pattern_uniform)) {
    for (uint i = 0; i < layer->start_inds.size(); i++) {
      int fst_idx = layer->start_inds[i];
      int lst_idx = 0;

      if (i < layer->start_inds.size() - 1)
        lst_idx = layer->start_inds[i+1] - 1;
      else
        lst_idx = layer->triangles.size() - 1;

      if (lst_idx <= fst_idx)
        continue;

      QPoint tex_ind;
      QSize tex_dim;

      if (!_is_pattern_uniform) {
        tex_ind = assets->getAreaPatternLocation(ref->getColorScheme(), layer->pattern_refs[i]);
        tex_dim = assets->getAreaPatternSize(ref->getColorScheme(), layer->pattern_refs[i]);
      }

      for (int j = fst_idx; j < lst_idx; j += 2) {
        if (!_is_color_uniform) {
          color_inds.push_back(layer->color_inds[i]);
        }

        if (!_is_pattern_uniform) {
          tex_inds.push_back(tex_ind.x());
          tex_inds.push_back(tex_ind.y());
          tex_dims.push_back(tex_dim.width());
          tex_dims.push_back(tex_dim.height());
        }
      }
    }
  }

  _point_count = layer->triangles.size() / 2;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, layer->triangles.size() * sizeof(GLfloat), layer->triangles.data(), GL_STATIC_DRAW);

  if (!_is_color_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_COLOR_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, color_inds.size() * sizeof(GLfloat), &color_inds[0], GL_STATIC_DRAW);
  }

  if (!_is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_PATTERN_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, tex_inds.size() * sizeof(GLfloat), &tex_inds[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_PATTERN_DIM]);
    glBufferData(GL_ARRAY_BUFFER, tex_dims.size() * sizeof(GLfloat), &tex_dims[0], GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChartAreaEngine::draw(ChartShaders* shaders) {
  if (_point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_COORDS]);
  glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COORDS));

  if (!_is_color_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_COLOR_INDEX]);
    glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX));
  } else {
    glVertexAttrib1f(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX), _color_ind);
    glDisableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX));
  }

  if (!_is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTRIBUTES_PATTERN_INDEX]);
    glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX));

    glBindBuffer(GL_ARRAY_BUFFER, AREA_ATTRIBUTES_PATTERN_DIM);
    glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM));
  } else {
    glVertexAttrib2f(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX), _patternLocation.x(), _patternLocation.y());
    glDisableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX));

    glVertexAttrib2f(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM), _patternSize.width(), _patternSize.height());
    glDisableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM));
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, _point_count);
}
