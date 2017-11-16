uniform sampler2D texture;

uniform float threashold;

varying float v_amp;

void main() {
  if (v_amp >= threashold) {
    gl_FragColor = texture2D(texture, vec2(0.0, v_amp / 255.0));
  } else {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  }
}
