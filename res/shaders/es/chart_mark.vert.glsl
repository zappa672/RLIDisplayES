#version 100

uniform mat4 mvp_matrix;

attribute vec2	coords;         // World position lat/lon in degrees
attribute vec2	vertex_offset;
attribute vec2	tex_coords;

uniform float north;            // Angle to north
uniform vec2  center;           // Chart center position lat/lon in degrees
uniform float scale;            // meters per pixel
uniform vec2  assetdim;         // Pattern texture full size in pixels
uniform float display_order;

varying vec2	v_texcoords;

const float EARTH_RAD_METERS = 6378137.0;

void main() {
  float lat_rads = radians(center.x);

  float y_m = -EARTH_RAD_METERS * radians(coords.x - center.x);
  float x_m =  EARTH_RAD_METERS * cos(lat_rads) * radians(coords.y - center.y);
  vec2  pix_pos = vec2(x_m, y_m) / scale;     // Screen

  v_texcoords = tex_coords / assetdim;
  gl_Position = mvp_matrix * vec4(pix_pos + vertex_offset, -display_order, 1.0);
}
