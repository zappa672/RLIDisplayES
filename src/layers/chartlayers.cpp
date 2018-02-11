#include "chartlayers.h"


ChartAreaEngine::ChartAreaEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;

  is_color_uniform = false;
  is_pattern_uniform = false;

  glGenBuffers(AREA_ATTRIBUTES_COUNT, vbo_ids);
}

ChartAreaEngine::~ChartAreaEngine() {
  glDeleteBuffers(AREA_ATTRIBUTES_COUNT, vbo_ids);
}

void ChartAreaEngine::clearData() {
  point_count = 0;

  for (int i = 0; i < AREA_ATTRIBUTES_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  }
}

void ChartAreaEngine::setData(S52AreaLayer* layer, S52Assets* assets, S52References* ref) {
  std::vector<GLfloat> color_inds;
  std::vector<GLfloat> tex_inds;
  std::vector<GLfloat> tex_dims;

  is_pattern_uniform = layer->is_pattern_uniform;
  patternLocation = assets->getAreaPatternLocation(ref->getColorScheme(), layer->pattern_ref);
  patternSize = assets->getAreaPatternSize(ref->getColorScheme(), layer->pattern_ref);

  is_color_uniform = layer->is_color_uniform;
  color_ind = layer->color_ind;

  if ((!is_color_uniform) || (!is_pattern_uniform)) {
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

      if (!is_pattern_uniform) {
        tex_ind = assets->getAreaPatternLocation(ref->getColorScheme(), layer->pattern_refs[i]);
        tex_dim = assets->getAreaPatternSize(ref->getColorScheme(), layer->pattern_refs[i]);
      }

      for (int j = fst_idx; j < lst_idx; j += 2) {
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

  point_count = layer->triangles.size() / 2;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, layer->triangles.size() * sizeof(GLfloat), &(layer->triangles[0]), GL_STATIC_DRAW);

  if (!is_color_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_COLOR_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, color_inds.size() * sizeof(GLfloat), &color_inds[0], GL_STATIC_DRAW);
  }

  if (!is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_PATTERN_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, tex_inds.size() * sizeof(GLfloat), &tex_inds[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_PATTERN_DIM]);
    glBufferData(GL_ARRAY_BUFFER, tex_dims.size() * sizeof(GLfloat), &tex_dims[0], GL_STATIC_DRAW);
  }
}

void ChartAreaEngine::draw(ChartShaders* shaders) {
  if (point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_COORDS]);
  glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COORDS));

  if (!is_color_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_COLOR_INDEX]);
    glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX));
  } else {
    glVertexAttrib1f(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX), color_ind);
    glDisableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_COLOR_INDEX));
  }

  if (!is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[AREA_ATTRIBUTES_PATTERN_INDEX]);
    glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX));

    glBindBuffer(GL_ARRAY_BUFFER, AREA_ATTRIBUTES_PATTERN_DIM);
    glVertexAttribPointer(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM));
  } else {
    glVertexAttrib2f(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX), patternLocation.x(), patternLocation.y());
    glDisableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX));

    glVertexAttrib2f(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_DIM), patternSize.width(), patternSize.height());
    glDisableVertexAttribArray(shaders->getAreaAttributeLoc(AREA_ATTRIBUTES_PATTERN_INDEX));
  }

  glDrawArrays(GL_TRIANGLES, 0, point_count);
}










ChartLineEngine::ChartLineEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;

  is_pattern_uniform = false;
  is_color_uniform = false;

  glGenBuffers(MARK_ATTRIBUTES_COUNT, vbo_ids);
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
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ChartLineEngine::setData(S52LineLayer* layer, S52Assets* assets, S52References* ref) {
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

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);

  glDrawElements(GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}









ChartMarkEngine::ChartMarkEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;
  is_pattern_uniform = false;

  glGenBuffers(MARK_ATTRIBUTES_COUNT, vbo_ids);
  glGenBuffers(1, &_ind_vbo_id);
}

ChartMarkEngine::~ChartMarkEngine() {
  glDeleteBuffers(MARK_ATTRIBUTES_COUNT, vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
}

void ChartMarkEngine::clearData() {
  point_count = 0;

  for (int i = 0; i < MARK_ATTRIBUTES_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ChartMarkEngine::setData(S52MarkLayer* layer, S52References* ref) {
  std::vector<GLfloat> world_coords;
  std::vector<GLfloat> vertex_orders;
  std::vector<GLfloat> symbol_origins;
  std::vector<GLfloat> symbol_sizes;
  std::vector<GLfloat> symbol_pivots;

  if (layer->is_symbol_uniform) {
    is_pattern_uniform = true;

    patternOrigin = ref->getSymbolIndex(layer->symbol_ref);
    patternSize = ref->getSymbolDim(layer->symbol_ref);
    patternPivot = ref->getSymbolPivot(layer->symbol_ref);
  }

  for (unsigned int i = 0; i < (layer->points.size() / 2); i ++) {
    QPointF tex_orig, tex_pivt;
    QSizeF tex_size;

    if (!is_pattern_uniform) {
      tex_orig = ref->getSymbolIndex(layer->symbol_refs[i]);
      tex_size = ref->getSymbolDim(layer->symbol_refs[i]);
      tex_pivt = ref->getSymbolPivot(layer->symbol_refs[i]);
    }

    for (int k = 0; k < 4; k++) {
      world_coords.push_back(layer->points[2*i+0]);
      world_coords.push_back(layer->points[2*i+1]);

      vertex_orders.push_back(k);

      if (!is_pattern_uniform) {
        symbol_origins.push_back(tex_orig.x());
        symbol_origins.push_back(tex_orig.y());
        symbol_sizes.push_back(tex_size.width());
        symbol_sizes.push_back(tex_size.height());
        symbol_pivots.push_back(tex_pivt.x());
        symbol_pivots.push_back(tex_pivt.y());
      }
    }
  }


  point_count = vertex_orders.size();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_WORLD_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, world_coords.size() * sizeof(GLfloat), &world_coords[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_VERTEX_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, vertex_orders.size() * sizeof(GLfloat), &vertex_orders[0], GL_STATIC_DRAW);

  if (!is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_ORIGIN]);
    glBufferData(GL_ARRAY_BUFFER, symbol_origins.size() * sizeof(GLfloat), &symbol_origins[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_SIZE]);
    glBufferData(GL_ARRAY_BUFFER, symbol_sizes.size() * sizeof(GLfloat), &symbol_sizes[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_PIVOT]);
    glBufferData(GL_ARRAY_BUFFER, symbol_pivots.size() * sizeof(GLfloat), &symbol_pivots[0], GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_PIVOT]);



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

void ChartMarkEngine::draw(ChartShaders* shaders) {
  if (point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_WORLD_COORDS]);
  glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_WORLD_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_WORLD_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_VERTEX_ORDER]);
  glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_VERTEX_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_VERTEX_ORDER));

  if (!is_pattern_uniform) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_ORIGIN]);
    glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_ORIGIN), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_ORIGIN));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_SIZE]);
    glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_SIZE), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_SIZE));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_SYMBOL_PIVOT]);
    glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_PIVOT), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_PIVOT));
  } else {
    glVertexAttrib2f(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_ORIGIN), patternOrigin.x(), patternOrigin.y());
    glDisableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_ORIGIN));

    glVertexAttrib2f(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_SIZE), patternSize.width(), patternSize.height());
    glDisableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_SIZE));

    glVertexAttrib2f(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_PIVOT), patternPivot.x(), patternPivot.y());
    glDisableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_SYMBOL_PIVOT));
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);

  glDrawElements( GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}













ChartSndgEngine::ChartSndgEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;

  glGenBuffers(SNDG_ATTRIBUTES_COUNT, vbo_ids);
  glGenBuffers(1, &_ind_vbo_id);
}

ChartSndgEngine::~ChartSndgEngine() {
  glDeleteBuffers(SNDG_ATTRIBUTES_COUNT, vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
}

void ChartSndgEngine::clearData() {
  point_count = 0;

  for (int i = 0; i < SNDG_ATTRIBUTES_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ChartSndgEngine::setData(S52SndgLayer* layer, S52Assets* assets, S52References* ref) {
  Q_UNUSED(assets);

  std::vector<GLfloat> world_coords;
  std::vector<GLfloat> vertex_orders;
  std::vector<GLfloat> symbol_orders;
  std::vector<GLfloat> symbol_fracs;
  std::vector<GLfloat> symbol_counts;
  std::vector<GLfloat> symbol_origins;
  std::vector<GLfloat> symbol_sizes;
  std::vector<GLfloat> symbol_pivots;

  for (unsigned int i = 0; i < (layer->points.size() / 2); i++) {
    QPoint tex_orig, tex_pivt;
    QSize tex_size;
    QString depth = QString::number(layer->depths[i], 'f', 1);

    bool frac = false;
    for (int j = 0; j < depth.length(); j++) {
      if (depth[j] == '.') {
        frac = true;
        continue;
      }

      if (frac && depth[j] == '0')
        continue;

      QString symbol_ref = "SOUNDS0" + depth[j];
      tex_orig = ref->getSymbolIndex(symbol_ref);
      tex_size = ref->getSymbolDim(symbol_ref);
      tex_pivt = ref->getSymbolPivot(symbol_ref);

      for (int k = 0; k < 4; k++) {
        world_coords.push_back(layer->points[2*i+0]);
        world_coords.push_back(layer->points[2*i+1]);

        vertex_orders.push_back(k);

        if (!frac) {
          symbol_orders.push_back(j);
          symbol_fracs.push_back(0);
        } else {
          symbol_orders.push_back(j-1);
          symbol_fracs.push_back(1);
        }

        symbol_counts.push_back(depth.length() - 2);

        symbol_origins.push_back(tex_orig.x());
        symbol_origins.push_back(tex_orig.y());

        symbol_sizes.push_back(tex_size.width());
        symbol_sizes.push_back(tex_size.height());

        symbol_pivots.push_back(tex_pivt.x());
        symbol_pivots.push_back(tex_pivt.y());
      }
    }
  }

  point_count = vertex_orders.size();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_WORLD_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, world_coords.size() * sizeof(GLfloat), &world_coords[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_VERTEX_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, vertex_orders.size() * sizeof(GLfloat), &vertex_orders[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, symbol_orders.size() * sizeof(GLfloat), &symbol_orders[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_FRAC]);
  glBufferData(GL_ARRAY_BUFFER, symbol_fracs.size() * sizeof(GLfloat), &symbol_fracs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_COUNT]);
  glBufferData(GL_ARRAY_BUFFER, symbol_counts.size() * sizeof(GLfloat), &symbol_counts[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_ORIGIN]);
  glBufferData(GL_ARRAY_BUFFER, symbol_origins.size() * sizeof(GLfloat), &symbol_origins[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_SIZE]);
  glBufferData(GL_ARRAY_BUFFER, symbol_sizes.size() * sizeof(GLfloat), &symbol_sizes[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_PIVOT]);
  glBufferData(GL_ARRAY_BUFFER, symbol_pivots.size() * sizeof(GLfloat), &symbol_pivots[0], GL_STATIC_DRAW);


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

void ChartSndgEngine::draw(ChartShaders* shaders) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_WORLD_COORDS]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_WORLD_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_WORLD_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_VERTEX_ORDER]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_VERTEX_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_VERTEX_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_ORDER]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_FRAC]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_FRAC), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_FRAC));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_COUNT]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_COUNT), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_COUNT));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_ORIGIN]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_ORIGIN), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_ORIGIN));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_SIZE]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_SIZE), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_SIZE));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[SNDG_ATTRIBUTES_SYMBOL_PIVOT]);
  glVertexAttribPointer(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_PIVOT), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getSndgAttributeLoc(SNDG_ATTRIBUTES_SYMBOL_PIVOT));


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);

  glDrawElements(GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}








ChartTextEngine::ChartTextEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;

  glGenBuffers(TEXT_ATTRIBUTES_COUNT, vbo_ids);
  glGenBuffers(1, &_ind_vbo_id);
}

ChartTextEngine::~ChartTextEngine() {
  glDeleteBuffers(TEXT_ATTRIBUTES_COUNT, vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);
}

void ChartTextEngine::clearData() {
  point_count = 0;

  for (int i = 0; i < TEXT_ATTRIBUTES_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ChartTextEngine::setData(S52TextLayer* layer) {
  std::vector<GLfloat> coords;
  std::vector<GLfloat> point_orders;
  std::vector<GLfloat> char_orders;
  std::vector<GLfloat> char_counts;
  std::vector<GLfloat> char_values;

  for (unsigned int i = 0; i < (layer->points.size() / 2); i ++) {
    QString txt = layer->texts[i];

    for (int j = 0; j < txt.size(); j++) {
      for (int k = 0; k < 4; k++) {
        coords.push_back(layer->points[2*i+0]);
        coords.push_back(layer->points[2*i+1]);

        point_orders.push_back(k);
        char_orders.push_back(j);
        char_counts.push_back(txt.length());
        char_values.push_back(static_cast<int>(txt.at(j).toLatin1()));
      }
    }
  }

  point_count = char_orders.size();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(GLfloat), &coords[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_POINT_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, point_orders.size() * sizeof(GLfloat), &point_orders[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, char_orders.size() * sizeof(GLfloat), &char_orders[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_COUNT]);
  glBufferData(GL_ARRAY_BUFFER, char_counts.size() * sizeof(GLfloat), &char_counts[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_VALUE]);
  glBufferData(GL_ARRAY_BUFFER, char_values.size() * sizeof(GLfloat), &char_values[0], GL_STATIC_DRAW);


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

void ChartTextEngine::draw(ChartShaders* shaders) {
  if (point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_COORDS]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_POINT_ORDER]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_POINT_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_POINT_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_ORDER]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_COUNT]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_COUNT), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_COUNT));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_VALUE]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_VALUE), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_VALUE));


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glDrawElements(GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
