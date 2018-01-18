uniform mat4 mvp_matrix;

// World position lat/lon in degrees
attribute vec2	world_coords;
// Quad vertex order (1 - bottom left, 2 - boottom right, 3 - top right, 4 - top left)
attribute float vertex_order;
attribute float	symbol_order;
attribute float symbol_frac;
attribute float symbol_count;
// Chart symbol position in pattern texture
attribute vec2	symbol_origin;
// Chart symbol size in pixels
attribute vec2	symbol_size;
// Chart symbol pivot point
attribute vec2	symbol_pivot;

// Angle to north
uniform float   north;
// Chart center position lat/lon in degrees
uniform vec2    center;
// meters / pixel
uniform float   scale;

// Pattern texture full size in pixels
uniform vec2  pattern_tex_size;

varying vec2	v_texcoords;

void main() {
  float lat_rads = radians(center.x);

  float y_m = 6378137.0*radians(world_coords.x - center.x);
  float x_m = 6378137.0*cos(lat_rads)*radians(world_coords.y - center.y);

  // screen position
  vec2 pos_pix = vec2(x_m, y_m) / scale;
  pos_pix = pos_pix + vec2(symbol_order * 8.0 - symbol_count * 4.0, symbol_frac * -4.0);

  // Variables to store symbol parameters
  vec2 origin, size, pivot;
  origin = symbol_origin;
  size = symbol_size;
  pivot = symbol_pivot + vec2(0.5, 0.5);

  // Quad vertex coordinates in pixels
  vec2 vert_pos_pix;

  // Quad vertex coordinates in pixels
  if (vertex_order == 0.0) {
    gl_Position = mvp_matrix * vec4(pos_pix + vec2(-pivot.x, pivot.y), 0.0, 1.0);
    v_texcoords = (pattern_tex_size*vec2(0.0, 1.0) + origin*vec2(1.0, -1.0) + size*vec2(0.0, -1.0)) / pattern_tex_size;
  } else if (vertex_order == 1.0) {
    gl_Position = mvp_matrix * vec4(pos_pix + vec2(-pivot.x, -(size.y-pivot.y)), 0.0, 1.0);
    v_texcoords = (pattern_tex_size*vec2(0.0, 1.0) + origin*vec2(1.0, -1.0) + size*vec2(0.0, 0.0)) / pattern_tex_size;
  } else if (vertex_order == 2.0) {
    gl_Position = mvp_matrix * vec4(pos_pix + vec2(size.x-pivot.x, -(size.y-pivot.y)), 0.0, 1.0);
    v_texcoords = (pattern_tex_size*vec2(0.0, 1.0) + origin*vec2(1.0, -1.0) + size*vec2(1.0, 0.0)) / pattern_tex_size;
  } else if (vertex_order == 3.0) {
    gl_Position = mvp_matrix * vec4(pos_pix + vec2(size.x-pivot.x, pivot.y), 0.0, 1.0);
    v_texcoords = (pattern_tex_size*vec2(0.0, 1.0) + origin*vec2(1.0, -1.0) + size*vec2(1.0, -1.0)) / pattern_tex_size;
  }
}