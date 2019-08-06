uniform mat4 mvp_matrix;

attribute float angle;
attribute float radius;

uniform float shift;

void main() {
  float phi = radians(angle);
  vec2 pos = radius * vec2(sin(phi), -cos(phi));

  if (shift != 0.0) {
    float phi_shift = radians(angle + 90.0);
    pos += shift * vec2(sin(phi_shift), -cos(phi_shift));
  }

  gl_Position = mvp_matrix * vec4(pos, 0.0, 1.0);
  gl_PointSize = 1.0;
}
