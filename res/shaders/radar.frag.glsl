//uniform sampler2D texture;

uniform vec3 palette[16];
uniform float threashold;

varying float v_amp;

void main() {
  if (v_amp >= threashold) {
    //gl_FragColor = texture2D(texture, vec2(0.0, v_amp / 255.0));
    gl_FragColor = vec4(palette[int(v_amp / 16.0)] / 255.0, 1.0);
  } else {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  }
}
