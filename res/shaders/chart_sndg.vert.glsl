uniform mat4 mvp_matrix;

// World position lat/lon in degrees
attribute vec2	coords;
// Quad vertex order (1 - bottom left, 2 - boottom right, 3 - top right, 4 - top left)
attribute float vertex_order;
attribute float	symbol_order;
attribute float symbol_frac;
attribute float symbol_count;
// Chart symbol position in pattern texture
attribute vec2	origin;
// Chart symbol size in pixels
attribute vec2	size;
// Chart symbol pivot point
attribute vec2	pivot;

// Angle to north
uniform float   north;
// Chart center position lat/lon in degrees
uniform vec2    center;
// meters / pixel
uniform float   scale;

// Pattern texture full size in pixels
uniform vec2  assetdim;

varying vec2	v_texcoords;

void main() {
  float lat_rads = radians(center.x);

  float y_m = -6378137.0 * radians(coords.x - center.x);
  float x_m = 6378137.0 * cos(lat_rads) * radians(coords.y - center.y);
  vec2  pix_pos = vec2(x_m, y_m) / scale;     // Screen pos

  pix_pos = pix_pos + vec2(symbol_order * 8.0 - symbol_count * 4.0, symbol_frac * -4.0);

  if (vertex_order == 0.0) {
    // Left-top
    gl_Position = mvp_matrix * vec4(pix_pos + vec2(-pivot.x, -pivot.y), 0.0, 1.0);
    v_texcoords = origin / assetdim;
  } else if (vertex_order == 1.0) {
    // Left-bottom
    gl_Position = mvp_matrix * vec4(pix_pos + vec2(size.x-pivot.x, -pivot.y), 0.0, 1.0);
    v_texcoords = (origin + vec2(size.x, 0.0)) / assetdim;
  } else if (vertex_order == 2.0) {
    // Right-bottom
    gl_Position = mvp_matrix * vec4(pix_pos + vec2(size.x-pivot.x, size.y-pivot.y), 0.0, 1.0);
    v_texcoords = (origin + size) / assetdim;
  } else if (vertex_order == 3.0) {
    // Right-top
    gl_Position = mvp_matrix * vec4(pix_pos + vec2(-pivot.x, size.y-pivot.y), 0.0, 1.0);
    v_texcoords = (origin + vec2(0.0, size.y)) / assetdim;
  }
}
