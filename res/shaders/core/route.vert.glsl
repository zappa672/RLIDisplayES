uniform mat4 mvp_matrix;

attribute vec2  world_coords;

uniform vec2 center;
uniform float scale;

const float EARTH_RAD_METERS = 6378137.0;

void main() {
  float lat_rads = radians(center.x);

  float y_m = -(EARTH_RAD_METERS / scale) * radians(world_coords.x - center.x);
  float x_m =  (EARTH_RAD_METERS / scale) * cos(lat_rads) * radians(world_coords.y - center.y);

  // screen position
  vec2 pix_pos = vec2(x_m, y_m);

  gl_Position = mvp_matrix  * vec4(pix_pos, 0.0, 1.0);
  gl_PointSize = 5.0;
}
