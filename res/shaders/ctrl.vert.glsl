uniform mat4 mvp_matrix;

attribute float angle;
attribute float radius;

void main() {
  float phi = radians(angle);
  vec2 pos = radius * vec2(float(cos(phi)), sin(phi));
  gl_Position = mvp_matrix * vec4(pos, 0.0, 1.0);
}
