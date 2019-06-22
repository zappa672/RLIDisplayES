#include "chartmarkengine.h"

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
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
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
      size = ref->getSymbolSize(layer->symbol_ref);
      pivt = ref->getSymbolPivot(layer->symbol_ref);
    } else {
      orig = ref->getSymbolIndex(layer->symbol_refs[i]);
      size = ref->getSymbolSize(layer->symbol_refs[i]);
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
      size = ref->getSymbolSize(symbol_ref);
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
