#version 100

uniform mat4 mvp_matrix;

attribute vec2 a_position;
attribute float a_amplitude;

varying float v_amp;

void main() {
  gl_Position = mvp_matrix * vec4(a_position, 0.0, 1.0);
  v_amp = a_amplitude;
  gl_PointSize = 1.0;
}
