#include "charttextengine.h"

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
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
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
