uniform mat4 mvp_matrix;

attribute vec2 position;
attribute vec4 color;

varying vec4 v_color;

void main(void) {
  gl_Position = mvp_matrix * vec4(position, 0.0, 1.0);
  v_color = color;
}
