#version 100

uniform sampler2D texture;
uniform vec4  color;
uniform float threashold;

varying float v_amp;

void main() {
  if (v_amp < threashold)
    gl_FragColor = color;
  else
    gl_FragColor = texture2D(texture, vec2(0.0, v_amp / 255.0));
}
