uniform mat4 mvp_matrix;

attribute float position;
attribute float amplitude;

uniform float peleng_length;
uniform float peleng_count;
uniform float north_shift;

varying float v_amp;

void main() {
  float peleng_index = floor(position / peleng_length);
  float radius = mod(position, peleng_length);

  float angle = radians(mod(north_shift + (360.0 * peleng_index) / peleng_count, 360.0));

  float x =  radius * sin(angle);
  float y = -radius * cos(angle);

  gl_Position = mvp_matrix * vec4(x, y, -amplitude, 1.0);
  if (length(gl_Position.xy) > 1.005)
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
  v_amp = amplitude;
}
