uniform mat4 mvp_matrix;

attribute vec2	coords1;
attribute vec2	coords2;
attribute float dist;
attribute float order;
attribute vec2	tex_orig;
attribute vec2	tex_dim;
attribute float color_index;

uniform float	north;
uniform vec2	center;
uniform float	scale;
uniform vec2  assetdim;
uniform float display_order;

varying float	v_color_index;
varying vec2	v_tex_dim;
varying vec2	v_tex_orig;
varying vec2	v_texcoords;
varying vec2	v_inner_texcoords;

void main() {
  float lat_rads = radians(center.x);

  float y1_m = -(6378137.0 / scale) * radians(coords1.x - center.x);
  float y2_m = -(6378137.0 / scale) * radians(coords2.x - center.x);

  float x1_m =  (6378137.0 / scale) * cos(lat_rads)*radians(coords1.y - center.y);
  float x2_m =  (6378137.0 / scale) * cos(lat_rads)*radians(coords2.y - center.y);

  // screen position
  vec2 pos1_pix = vec2(x1_m, y1_m);
  vec2 pos2_pix = vec2(x2_m, y2_m);

  v_tex_dim = tex_dim;
  v_tex_orig = tex_orig;
  v_color_index = color_index;
  v_texcoords = assetdim;
  float dist_pix = dist / scale;

  vec2 tan_pix = pos2_pix - pos1_pix;
  float len_pix = sqrt(tan_pix.x * tan_pix.x + tan_pix.y * tan_pix.y);
  vec2 unit_tan_pix = normalize(tan_pix);
  vec2 norm_pix = (v_tex_dim.y / 2.0) * vec2(unit_tan_pix.y, -unit_tan_pix.x);

  if (order == 0.0) {
    gl_Position = mvp_matrix * vec4(pos1_pix - norm_pix, -display_order, 1.0);
    v_inner_texcoords = vec2(dist_pix/v_tex_dim.x, 0.0);
  } else if (order == 1.0) {
    gl_Position = mvp_matrix * vec4(pos2_pix - norm_pix, -display_order, 1.0);
    v_inner_texcoords = vec2((dist_pix + len_pix)/v_tex_dim.x, 0.0);
  } else if (order == 2.0) {
    gl_Position = mvp_matrix * vec4(pos2_pix + norm_pix, -display_order, 1.0);
    v_inner_texcoords = vec2((dist_pix + len_pix)/v_tex_dim.x, 1.0);
  } else if (order == 3.0) {
    gl_Position = mvp_matrix * vec4(pos1_pix + norm_pix, -display_order, 1.0);
    v_inner_texcoords = vec2(dist_pix/v_tex_dim.x, 1.0);
  }
}
