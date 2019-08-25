#include "chartareaengine.h"

ChartAreaEngine::ChartAreaEngine(QOpenGLContext* context) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  _point_count = 0;

  glGenBuffers(AREA_ATTR_COUNT, _vbo_ids);
}

ChartAreaEngine::~ChartAreaEngine() {
  glDeleteBuffers(AREA_ATTR_COUNT, _vbo_ids);
}

void ChartAreaEngine::clearData() {
  _point_count = 0;

  for (int i = 0; i < AREA_ATTR_COUNT; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[i]);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChartAreaEngine::setData(S52::AreaLayer* layer, S52Assets* assets, S52References* ref, int display_order) {
  _display_order = display_order;

  std::vector<GLfloat> color_inds;
  std::vector<GLfloat> tex_inds;
  std::vector<GLfloat> tex_dims;

  for (size_t i = 0; i < layer->start_inds.size(); i++) {
    size_t fst_idx = layer->start_inds[i];
    size_t lst_idx = 0;

    if (i < layer->start_inds.size() - 1)
      lst_idx = layer->start_inds[i+1] - 1;
    else
      lst_idx = layer->triangles.size() - 1;

    if (lst_idx <= fst_idx)
      continue;

    QPoint tex_ind = assets->getAreaPatternLocation(ref->getColorScheme(), layer->pattern_refs[i]);
    QSize tex_dim = assets->getAreaPatternSize(ref->getColorScheme(), layer->pattern_refs[i]);

    for (size_t j = fst_idx; j < lst_idx; j += 2) {
      color_inds.push_back(layer->color_inds[i]);

      tex_inds.push_back(tex_ind.x());
      tex_inds.push_back(tex_ind.y());
      tex_dims.push_back(tex_dim.width());
      tex_dims.push_back(tex_dim.height());
    }
  }

  _point_count = layer->triangles.size() / 2;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, layer->triangles.size() * sizeof(GLfloat), layer->triangles.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_COLOR_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, color_inds.size() * sizeof(GLfloat), &color_inds[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_PATTERN_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, tex_inds.size() * sizeof(GLfloat), &tex_inds[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_PATTERN_DIM]);
  glBufferData(GL_ARRAY_BUFFER, tex_dims.size() * sizeof(GLfloat), &tex_dims[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChartAreaEngine::draw(ChartShaders* shaders) {
  if (_point_count <= 0)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_COORDS]);
  glVertexAttribPointer(shaders->getAreaAttrLoc(AREA_ATTR_COORDS), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getAreaAttrLoc(AREA_ATTR_COORDS));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_COLOR_INDEX]);
  glVertexAttribPointer(shaders->getAreaAttrLoc(AREA_ATTR_COLOR_INDEX), 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getAreaAttrLoc(AREA_ATTR_COLOR_INDEX));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[AREA_ATTR_PATTERN_INDEX]);
  glVertexAttribPointer(shaders->getAreaAttrLoc(AREA_ATTR_PATTERN_INDEX), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getAreaAttrLoc(AREA_ATTR_PATTERN_INDEX));

  glBindBuffer(GL_ARRAY_BUFFER, AREA_ATTR_PATTERN_DIM);
  glVertexAttribPointer(shaders->getAreaAttrLoc(AREA_ATTR_PATTERN_DIM), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glEnableVertexAttribArray(shaders->getAreaAttrLoc(AREA_ATTR_PATTERN_DIM));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, _point_count);
}
