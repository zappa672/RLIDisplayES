uniform mat4 mvp_matrix;

attribute vec2  position;
attribute float order;
attribute float char_val;

uniform vec2  size;

varying float	v_char_val;
varying vec2	v_texcoords;


void main() {
  vec2 tex_coord = (1.0/16.0) * vec2( mod(char_val , 16.0), floor(char_val / 16.0));

  if (order == 0.0) {
    gl_Position = mvp_matrix * vec4(position, 0.0, 1.0);
    v_texcoords = tex_coord + vec2(0.0, 0.0);
  } else if (order == 1.0) {
    gl_Position = mvp_matrix * vec4(position + vec2(0, size.y), 0.0, 1.0);
    v_texcoords = tex_coord + vec2(0.0, 1.0 / 16.0);
  } else if (order == 2.0) {
    gl_Position = mvp_matrix * vec4(position + vec2(size.x, 0.0), 0.0, 1.0);
    v_texcoords = tex_coord + vec2(1.0 / 16.0, 0.0);
  } else if (order == 3.0) {
    gl_Position = mvp_matrix * vec4(position + size, 0.0, 1.0);
    v_texcoords = tex_coord + vec2(1.0 / 16.0, 1.0 / 16.0);
  }

  v_char_val = char_val;
}
