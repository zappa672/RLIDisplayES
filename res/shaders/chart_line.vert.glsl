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

uniform float	u_color_table[256];

uniform float u_color_index;
uniform vec2  u_tex_origin;
uniform vec2  u_tex_dim;

varying vec3	v_color;
varying vec2	v_tex_dim;
varying vec2	v_tex_orig;
varying vec2	v_texcoords;
varying vec2	v_inner_texcoords;
varying float v_use_tex_color;

void main() {
  float lat_rads = radians(center.x);

  float y1_m = 6378137.0 * radians(coords1.x - center.x);
  float y2_m = 6378137.0 * radians(coords2.x - center.x);

  float x1_m = 6378137.0 * cos(lat_rads)*radians(coords1.y - center.y);
  float x2_m = 6378137.0 * cos(lat_rads)*radians(coords2.y - center.y);

  // screen position
  float pix_per_meter = 1.0 / scale;
  vec2 pos1_pix = vec2(x1_m, y1_m) / scale;
  vec2 pos2_pix = vec2(x2_m, y2_m) / scale;


  vec2 tan_pix = pos2_pix - pos1_pix;
  float len_pix = sqrt(tan_pix.x * tan_pix.x + tan_pix.y * tan_pix.y);
  vec2 unit_tan_pix = normalize(tan_pix);

  if (u_tex_dim.r == -1.0) {
    v_tex_dim = tex_dim;
    v_tex_orig = tex_orig;
  } else {
    v_tex_dim = u_tex_dim;
    v_tex_orig = u_tex_origin;
  }

  vec2 norm_pix = (v_tex_dim.y / 2.0) * vec2(unit_tan_pix.y, -unit_tan_pix.x);

  float color_ind = 0.0;
  if (u_color_index == -1.0)
    color_ind = color_index;
  else
    color_ind = u_color_index;
  v_color = vec3(u_color_table[int(3.0*color_ind+0.0)], u_color_table[int(3.0*color_ind+1.0)], u_color_table[int(3.0*color_ind+2.0)]);

  v_texcoords = assetdim;
  float dist_pix = dist * pix_per_meter;

  if (order == 0.0) {
    gl_Position = mvp_matrix * vec4(pos1_pix - norm_pix, 0.0, 1.0);
    v_inner_texcoords = vec2(dist_pix/v_tex_dim.x, 0.0);
  } else if (order == 1.0) {
    gl_Position = mvp_matrix * vec4(pos2_pix - norm_pix, 0.0, 1.0);
    v_inner_texcoords = vec2((dist_pix + len_pix)/v_tex_dim.x, 0.0);
  } else if (order == 2.0) {
    gl_Position = mvp_matrix * vec4(pos2_pix + norm_pix, 0.0, 1.0);
    v_inner_texcoords = vec2((dist_pix + len_pix)/v_tex_dim.x, 1.0);
  } else if (order == 3.0) {
    gl_Position = mvp_matrix * vec4(pos1_pix + norm_pix, 0.0, 1.0);
    v_inner_texcoords = vec2(dist_pix/v_tex_dim.x, 1.0);
  }

  if (tex_orig.y < 96.0)
    v_use_tex_color = 1.0;
  else
    v_use_tex_color = 0.0;
}
