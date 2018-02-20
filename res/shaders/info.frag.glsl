uniform vec4  color;

varying float v_char_val;
varying vec2	v_texcoords;

uniform sampler2D glyph_tex;

void main() {
  if (v_char_val != 0.0) {
    vec4 tex_color = texture2D(glyph_tex, v_texcoords);
    gl_FragColor = color*tex_color;
  } else {
    gl_FragColor = color;
  }
}
