uniform mat4 mvp_matrix;

// World position lat/lon in degrees
attribute vec2	coords;
attribute float	point_order;

attribute float	char_shift;
attribute float	char_val;     // Character integer value in cp866

uniform float   north;          // Angle to north
uniform vec2    center;         // Chart center position lat/lon in degrees
uniform float   scale;          // meters / pixel
uniform float   display_order;

varying vec2 v_texcoord;

const float EARTH_RAD_METERS = 6378137.0;


void main() {
  float lat_rads = radians(center.x);

  float y = -(EARTH_RAD_METERS / scale) * radians(coords.x - center.x);
  float x =  (EARTH_RAD_METERS / scale) * cos(lat_rads)*radians(coords.y - center.y);
  x = x + char_shift;

  vec2 texcoord = vec2(mod(char_val, 16.0), floor(char_val / 16.0)) / 16.0;

  if (point_order == 0.0) {
    gl_Position = mvp_matrix * vec4(x - 8.0, y - 8.0, -1000.0, 1.0);
    v_texcoord = texcoord;
  } else if (point_order == 1.0) {
    gl_Position = mvp_matrix * vec4(x - 8.0, y + 8.0, -1000.0, 1.0);
    v_texcoord = texcoord + vec2(0.0, 1.0/16.0);
  } else if (point_order == 2.0) {
    gl_Position = mvp_matrix * vec4(x + 8.0, y + 8.0, -1000.0, 1.0);
    v_texcoord = texcoord + vec2(1.0/16.0, 1.0/16.0);
  } else if (point_order == 3.0) {
    gl_Position = mvp_matrix * vec4(x + 8.0, y - 8.0, -1000.0, 1.0);
    v_texcoord = texcoord + vec2(1.0/16.0, 0.0);
  }
}
