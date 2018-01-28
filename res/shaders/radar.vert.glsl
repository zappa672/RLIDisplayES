uniform mat4 mvp_matrix;

//attribute vec2 position;
attribute float position;
attribute float amplitude;

uniform float peleng_length;
uniform float peleng_count;

varying float v_amp;

const float TWO_PI = 2.0*3.1415926535897932384626433832795;

void main() {
  float peleng_index = floor(position / peleng_length);
  float radius = mod(position, peleng_length);

  float angle = (TWO_PI * peleng_index) / peleng_count;

  float x = radius * sin(angle);
  float y = -radius * cos(angle);

  gl_Position = mvp_matrix * vec4(x, y, -amplitude, 1.0);

  v_amp = amplitude;
}
