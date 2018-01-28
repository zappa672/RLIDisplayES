uniform mat4 mvp_matrix;

attribute vec2	coords;
attribute float color_index;
attribute vec2	tex_origin;
attribute vec2	tex_dim;

uniform float	north;
uniform vec2	center;
uniform float	scale;
uniform vec2  assetdim;

uniform float u_color_index;
uniform vec2  u_tex_origin;
uniform vec2  u_tex_dim;


varying float v_color_index;
varying vec2	v_tex_dim;
varying vec2	v_tex_orig;
varying vec2	v_inner_texcoords;
varying float v_use_tex;

void main() {
  float lat_rads = radians(center.x);

  float y_m = 6378137.0 * radians(coords.x - center.x);
  float x_m = 6378137.0 * cos(lat_rads) * radians(coords.y - center.y);

  // screen position
  vec2 pix_pos = vec2(x_m, y_m) / scale;
  gl_Position = mvp_matrix  * vec4(pix_pos, 0.0, 1.0);

  if (u_tex_dim.r == -1.0) {
    //0 - 1
    v_inner_texcoords = pix_pos / tex_dim;
    v_tex_dim = tex_dim / assetdim;
    v_tex_orig = tex_origin / assetdim;
  } else {
    //0 - 1
    v_inner_texcoords = pix_pos / u_tex_dim;
    v_tex_dim = u_tex_dim / assetdim;
    v_tex_orig = u_tex_origin / assetdim;
  }

  float color_ind = 0.0;
  if (u_color_index == -1.0)
    color_ind = color_index;
  else
    color_ind = u_color_index;

  // !!!! check to SOLID pattern - to replace!!!!
  if (v_tex_orig != vec2(0.0, 0.0))
    v_use_tex = 1.0;
  else
    v_use_tex = 0.0;

  v_color_index = color_ind;
}
