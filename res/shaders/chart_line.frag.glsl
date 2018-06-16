varying float v_color_index;
varying vec2	v_tex_dim;
varying vec2	v_tex_orig;
varying vec2	v_texcoords;
varying vec2	v_inner_texcoords;

uniform sampler2D pattern_tex;
uniform sampler2D color_table_tex;

void main() {
  vec2 texcoords = (v_tex_orig + (fract(v_inner_texcoords) * v_tex_dim)) / v_texcoords;
  vec4 tex_color = texture2D(pattern_tex, texcoords);
  vec4 color = texture2D(color_table_tex, vec2(0.0, (v_color_index + 0.5) / 63.0));

  //gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  gl_FragColor = tex_color * color;
  //gl_FragColor = tex_color;
}
