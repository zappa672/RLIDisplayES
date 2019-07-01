#include "chartlineengine.h"

ChartLineEngine::ChartLineEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;

  is_pattern_uniform = false;
  is_color_uniform = false;

  glGenBuffers(LINE_ATTRIBUTES_COUNT, vbo_ids);
  glGenBuffers(1, &_ind_vbo_id);
}

ChartLineEngine::~ChartLineEngine() {
  glDeleteBuffers(LINE_ATTRIBUTES_COUNT, vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
}

void ChartLineEngine::clearData() {
  point_count = 0;

  for (int i = 0; i < LINE_ATTRIBUTES_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ChartLineEngine::setData(S52LineLayer* layer, S52Assets* assets, S52References* ref, int display_order) {
  _display_order = display_order;

  std::vector<GLfloat> coords1;
  std::vector<GLfloat> coords2;
  std::vector<GLfloat> point_ords;
  std::vector<GLfloat> distances;
  std::vector<GLfloat> color_inds;
  std::vector<GLfloat> tex_inds;
  std::vector<GLfloat> tex_dims;

  if (layer->is_pattern_uniform) {
    is_pattern_uniform = true;
    patternIdx = assets->getLinePatternLocation(ref->getColorScheme(), layer->pattern_ref);
    patternDim = assets->getLinePatternSize(ref->getColorScheme(), layer->pattern_ref);
  }

  if (layer->is_color_uniform) {
    is_color_uniform = true;
    color_ind = layer->color_ind;
  }

  for (unsigned int i = 0; i < layer->start_inds.size(); i++) {
    int fst_idx = layer->start_inds[i];
    int lst_idx = 0;

    if (i < layer->start_inds.size() - 1)
      lst_idx = layer->start_inds[i+1] - 1;
    else
      lst_idx = layer->points.size() - 1;

    if (lst_idx <= fst_idx)
      continue;

    QPoint tex_ind;
    QSize tex_dim;
    if (!is_pattern_uniform) {
      tex_ind = assets->getLinePatternLocation(ref->getColorScheme(), layer->pattern_refs[i]);
      tex_dim = assets->getLinePatternSize(ref->getColorScheme(), layer->pattern_refs[i]);
    }

    for (int j = fst_idx; j < lst_idx - 2; j += 2) {
      for (int k = 0; k < 4; k++) {
        coords1.push_back(layer->points[j+0]);
        coords1.push_back(layer->points[j+1]);
        coords2.push_back(layer->points[j+2]);
        coords2.push_back(layer->points[j+3]);
        point_ords.push_back(k);
        distances.push_back(layer->distances[j/2]);

        if (!is_color_uniform) {
          color_inds.push_back(layer->color_inds[i]);
        }

        if (!is_pattern_uniform) {
          tex_inds.push_back(tex_ind.x());
          tex_inds.push_back(tex_ind.y());
          tex_dims.push_back(tex_dim.width());
          tex_dims.push_back(tex_dim.height());
        }
      }
    }
  }

  point_count = point_ords.size();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_COORDS1]);
  glBufferData(GL_ARRAY_BUFFER, coords1.size() * sizeof(GLfloat), &coords1[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_COORDS2]);
  glBufferData(GL_ARRAY_BUFFER, coords2.size() * sizeof(GLfloat), &coords2[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_DISTANCE]);
  glBufferData(GL_ARRAY_BUFFER, distances.size() * sizeof(GLfloat), &distances[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, point_ords.size() * sizeof(GLfloat), &point_ords[0], GL_STATIC_DRAW);

  if (!is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_PATTERN_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, tex_inds.size() * sizeof(GLfloat), &tex_inds[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_PATTERN_DIM]);
    glBufferData(GL_ARRAY_BUFFER, tex_dims.size() * sizeof(GLfloat), &tex_dims[0], GL_STATIC_DRAW);
  }

  if (!is_color_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_COLOR_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, color_inds.size() * sizeof(GLfloat), &color_inds[0], GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  std::vector<GLuint> draw_indices;

  for (GLuint i = 0; i < point_count; i += 4) {
    draw_indices.push_back(i);
    draw_indices.push_back(i+1);
    draw_indices.push_back(i+2);
    draw_indices.push_back(i);
    draw_indices.push_back(i+2);
    draw_indices.push_back(i+3);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw_indices.size()*sizeof(GLuint), draw_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ChartLineEngine::draw(ChartShaders* shaders) {
  if (point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_COORDS1]);
  glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COORDS1), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COORDS1));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_COORDS2]);
  glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COORDS2), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COORDS2));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_DISTANCE]);
  glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_DISTANCE), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_DISTANCE));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_ORDER]);
  glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_ORDER));

  if (!is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_PATTERN_INDEX]);
    glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_INDEX), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_INDEX));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_PATTERN_DIM]);
    glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_DIM), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_DIM));
  } else {
    glVertexAttrib2f(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_INDEX), patternIdx.x(), patternIdx.y());
    glVertexAttrib2f(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_DIM), patternDim.width(), patternDim.height());

    glDisableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_INDEX));
    glDisableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_PATTERN_DIM));
  }

  if (!is_color_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[LINE_ATTRIBUTES_COLOR_INDEX]);
    glVertexAttribPointer(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COLOR_INDEX), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COLOR_INDEX));
  } else {
    glVertexAttrib1f(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COLOR_INDEX), color_ind);
    glDisableVertexAttribArray(shaders->getLineAttributeLoc(LINE_ATTRIBUTES_COLOR_INDEX));
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glDrawElements(GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}