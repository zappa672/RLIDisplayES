#version 100

varying vec2	v_texcoords;

uniform sampler2D pattern_tex;

void main() {
  gl_FragColor = texture2D(pattern_tex, v_texcoords);
}
