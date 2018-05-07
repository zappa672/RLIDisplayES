#include "chartlayers.h"


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
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
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









ChartMarkEngine::ChartMarkEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  point_count = 0;

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
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ChartMarkEngine::setData(S52MarkLayer* layer, S52References* ref, int display_order) {
  _display_order = display_order;

  std::vector<GLfloat> world_coords;

  std::vector<GLfloat> vertex_offsets;
  std::vector<GLfloat> tex_coords;

  QPointF orig, pivt;
  QSizeF size;

  QPointF vertex_offset;
  QPointF tex_coord;

  for (unsigned int i = 0; i < (layer->points.size() / 2); i++) {
    if (layer->is_uniform) {
      orig = ref->getSymbolIndex(layer->symbol_ref);
      size = ref->getSymbolDim(layer->symbol_ref);
      pivt = ref->getSymbolPivot(layer->symbol_ref);
    } else {
      orig = ref->getSymbolIndex(layer->symbol_refs[i]);
      size = ref->getSymbolDim(layer->symbol_refs[i]);
      pivt = ref->getSymbolPivot(layer->symbol_refs[i]);
    }

    for (int k = 0; k < 4; k++) {
      world_coords.push_back(layer->points[2*i+0]);
      world_coords.push_back(layer->points[2*i+1]);

      switch (k) {
      case 0:
        vertex_offset = -pivt;
        tex_coord = orig;
        break;
      case 1:
        vertex_offset = QPointF(size.width() - pivt.x(), -pivt.y());
        tex_coord = QPointF(size.width() + orig.x(), orig.y());
        break;
      case 2:
        vertex_offset = QPointF(size.width() - pivt.x(), size.height() - pivt.y());
        tex_coord = QPointF(size.width() + orig.x(), size.height() + orig.y());
        break;
      case 3:
        vertex_offset = QPointF(-pivt.x(), size.height() - pivt.y());
        tex_coord = QPointF(orig.x(), size.height() + orig.y());
        break;
      }

      vertex_offsets.push_back(vertex_offset.x());
      vertex_offsets.push_back(vertex_offset.y());
      tex_coords.push_back(tex_coord.x());
      tex_coords.push_back(tex_coord.y());
    }
  }

  point_count = world_coords.size() / 2;

  setupBuffers(world_coords, vertex_offsets, tex_coords);
}

void ChartMarkEngine::setupBuffers( const std::vector<GLfloat>& world_coords
                                  , const std::vector<GLfloat>& vertex_offsets
                                  , const std::vector<GLfloat>& tex_coords )
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_WORLD_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, world_coords.size() * sizeof(GLfloat), &world_coords[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_VERTEX_OFFSET]);
  glBufferData(GL_ARRAY_BUFFER, vertex_offsets.size() * sizeof(GLfloat), &vertex_offsets[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_TEX_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(GLfloat), &tex_coords[0], GL_STATIC_DRAW);

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

void ChartMarkEngine::setData(S52SndgLayer* layer, S52Assets* assets, S52References* ref, int display_order) {
  Q_UNUSED(assets);

  _display_order = display_order;

  std::vector<GLfloat> world_coords;
  std::vector<GLfloat> vertex_offsets;
  std::vector<GLfloat> tex_coords;

  QPoint orig, pivt;
  QSize size;

  QPointF vertex_offset;
  QPointF tex_coord;


  for (unsigned int i = 0; i < (layer->points.size() / 2); i++) {
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
      orig = ref->getSymbolIndex(symbol_ref);
      size = ref->getSymbolDim(symbol_ref);
      pivt = ref->getSymbolPivot(symbol_ref);

      for (int k = 0; k < 4; k++) {
        world_coords.push_back(layer->points[2*i+0]);
        world_coords.push_back(layer->points[2*i+1]);

        int symbol_count = depth.length() - 2;

        if (!frac)
          vertex_offset = QPointF(j * 8.0 - symbol_count * 4.0, 0.0);
        else
          vertex_offset = QPointF((j-1) * 8.0 - symbol_count * 4.0, -4.0);

        switch (k) {
        case 0:
          vertex_offset += -pivt;
          tex_coord = orig;
          break;
        case 1:
          vertex_offset += QPointF(size.width() - pivt.x(), -pivt.y());
          tex_coord = QPointF(size.width() + orig.x(), orig.y());
          break;
        case 2:
          vertex_offset += QPointF(size.width() - pivt.x(), size.height() - pivt.y());
          tex_coord = QPointF(size.width() + orig.x(), size.height() + orig.y());
          break;
        case 3:
          vertex_offset += QPointF(-pivt.x(), size.height() - pivt.y());
          tex_coord = QPointF(orig.x(), size.height() + orig.y());
          break;
        }

        vertex_offsets.push_back(vertex_offset.x());
        vertex_offsets.push_back(vertex_offset.y());
        tex_coords.push_back(tex_coord.x());
        tex_coords.push_back(tex_coord.y());
      }
    }
  }

  point_count = world_coords.size() / 2;

  setupBuffers(world_coords, vertex_offsets, tex_coords);
}

void ChartMarkEngine::draw(ChartShaders* shaders) {
  if (point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_WORLD_COORDS]);
  glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_WORLD_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_WORLD_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_VERTEX_OFFSET]);
  glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_VERTEX_OFFSET), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_VERTEX_OFFSET));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[MARK_ATTRIBUTES_TEX_COORDS]);
  glVertexAttribPointer(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_TEX_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getMarkAttributeLoc(MARK_ATTRIBUTES_TEX_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glDrawElements(GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ChartTextEngine::setData(S52TextLayer* layer, int display_order) {
  _display_order = display_order;

  std::vector<GLfloat> coords;
  std::vector<GLfloat> point_orders;
  std::vector<GLfloat> char_shifts;
  std::vector<GLfloat> char_values;

  for (unsigned int i = 0; i < (layer->points.size() / 2); i ++) {
    QString txt = layer->texts[i];
    int strlen = txt.length();

    for (int j = 0; j < txt.size(); j++) {
      for (int k = 0; k < 4; k++) {
        coords.push_back(layer->points[2*i+0]);
        coords.push_back(layer->points[2*i+1]);

        point_orders.push_back(k);
        char_shifts.push_back(j * 8.0 - strlen * 4.0);
        char_values.push_back(static_cast<int>(txt.at(j).toLatin1()));
      }
    }
  }

  point_count = point_orders.size();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(GLfloat), coords.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_POINT_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, point_orders.size() * sizeof(GLfloat), point_orders.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_SHIFT]);
  glBufferData(GL_ARRAY_BUFFER, char_shifts.size() * sizeof(GLfloat), char_shifts.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_VALUE]);
  glBufferData(GL_ARRAY_BUFFER, char_values.size() * sizeof(GLfloat), char_values.data(), GL_STATIC_DRAW);

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

void ChartTextEngine::draw(ChartShaders* shaders) {
  if (point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_COORDS]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_POINT_ORDER]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_POINT_ORDER), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_POINT_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_SHIFT]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_SHIFT), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_SHIFT));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXT_ATTRIBUTES_CHAR_VALUE]);
  glVertexAttribPointer(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_VALUE), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getTextAttributeLoc(TEXT_ATTRIBUTES_CHAR_VALUE));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glDrawElements(GL_TRIANGLES, 3*(point_count/2), GL_UNSIGNED_INT, (const GLvoid*)(0 * sizeof(GLuint)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
