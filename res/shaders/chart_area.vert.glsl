uniform mat4 mvp_matrix;

attribute vec2	coords;
attribute float color_index;
attribute vec2	tex_origin;
attribute vec2	tex_dim;

uniform float	north;
uniform vec2	center;
uniform float	scale;
uniform vec2  assetdim;


varying float v_color_index;
varying vec2	v_tex_dim;
varying vec2	v_tex_orig;
varying vec2	v_inner_texcoords;

void main() {
  float lat_rads = radians(center.x);

  float y_m = -(6378137.0 / scale) * radians(coords.x - center.x);
  float x_m =  (6378137.0 / scale) * cos(lat_rads) * radians(coords.y - center.y);

  // screen position
  vec2 pix_pos = vec2(x_m, y_m);
  gl_Position = mvp_matrix  * vec4(pix_pos, 0.0, 1.0);

  if (tex_origin.x >= 0.0) {
    v_inner_texcoords = pix_pos / tex_dim;
    v_tex_dim = tex_dim / assetdim;
    v_tex_orig = tex_origin / assetdim;
  } else {
    v_tex_orig = vec2(-1.0, -1.0);
  }

  v_color_index = color_index;
}
