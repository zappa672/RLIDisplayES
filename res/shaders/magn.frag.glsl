uniform sampler2D texture;
uniform vec4  color;

varying float v_amp;

void main() {
  if (v_amp < 0.0)
    gl_FragColor = color;
  else
    gl_FragColor = texture2D(texture, vec2(0.0, v_amp / 255.0));

  //gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}


