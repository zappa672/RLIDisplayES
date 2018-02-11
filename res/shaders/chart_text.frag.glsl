varying vec2 v_texcoord;

uniform sampler2D glyph_tex;

void main() {
  gl_FragColor = texture2D(glyph_tex, v_texcoord);
}
