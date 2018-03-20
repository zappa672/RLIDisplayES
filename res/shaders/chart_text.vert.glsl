uniform mat4 mvp_matrix;

// World position lat/lon in degrees
attribute vec2	coords;
attribute float	point_order;
// Character int value and order in text line
attribute float	char_order;
attribute float	char_count;
attribute float	char_val;

// Angle to north
uniform float   north;
// Chart center position lat/lon in degrees
uniform vec2    center;
// meters / pixel
uniform float   scale;
uniform float   display_order;


varying vec2 v_texcoord;


void main() {
  float lat_rads = radians(center.x);

  float y = -(6378137.0 / scale) * radians(coords.x - center.x);
  float x =  (6378137.0 / scale) * cos(lat_rads)*radians(coords.y - center.y);
  x = x + char_order * 8.0 - char_count * 4.0;

  vec2 texcoord = vec2(mod(char_val, 16.0), floor(char_val / 16.0)) / 16.0;

  if (point_order == 0.0) {
    gl_Position = mvp_matrix * vec4(x - 8.0, y - 8.0, -display_order, 1.0);
    v_texcoord = texcoord;
  } else if (point_order == 1.0) {
    gl_Position = mvp_matrix * vec4(x - 8.0, y + 8.0, -display_order, 1.0);
    v_texcoord = texcoord + vec2(0.0, 1.0/16.0);
  } else if (point_order == 2.0) {
    gl_Position = mvp_matrix * vec4(x + 8.0, y + 8.0, -display_order, 1.0);
    v_texcoord = texcoord + vec2(1.0/16.0, 1.0/16.0);
  } else if (point_order == 3.0) {
    gl_Position = mvp_matrix * vec4(x + 8.0, y - 8.0, -display_order, 1.0);
    v_texcoord = texcoord + vec2(1.0/16.0, 0.0);
  }
}
