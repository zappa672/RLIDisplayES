varying float v_color_index;
varying vec2	v_tex_dim;
varying vec2	v_tex_orig;
varying vec2	v_inner_texcoords;

uniform sampler2D pattern_tex;
uniform sampler2D color_table_tex;

void main() {
  vec4 color = texture2D(color_table_tex, vec2(0.0, (v_color_index + 0.5) / 63.0));

  if (v_tex_orig.x >= 0.0) {
    vec2 texcoords = v_tex_orig + (fract(v_inner_texcoords) * v_tex_dim);
    vec4 tex_color = texture2D(pattern_tex, texcoords);
    gl_FragColor = color * tex_color;
  } else {
    gl_FragColor = color;
  }
}
