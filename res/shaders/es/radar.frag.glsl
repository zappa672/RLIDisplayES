#version 100

uniform sampler2D texture;

uniform float threashold;
uniform float fbo_radius;

varying float v_amp;

void main() {
  if (length(gl_FragCoord.xy - vec2(fbo_radius, fbo_radius)) > fbo_radius)
    discard;

  if (v_amp >= threashold) {
    gl_FragColor = texture2D(texture, vec2(0.0, v_amp / 255.0));
  } else {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);
  }
}
