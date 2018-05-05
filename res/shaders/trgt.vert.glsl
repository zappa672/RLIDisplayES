uniform mat4 mvp_matrix;

attribute vec2  world_coords;
attribute float vertex_order;
attribute float heading;
attribute float rotation;
attribute float course;
attribute float speed;

uniform vec2 center;
uniform float scale;
uniform float type;

varying vec2 v_inner_texcoords;
varying float v_type;

void main() {
  float lat_rads = radians(center.x);

  float y_m = -6378137.0 * radians(world_coords.x - center.x);
  float x_m = 6378137.0 * cos(lat_rads)*radians(world_coords.y - center.y);

  // screen position
  vec2 pix_pos = vec2(x_m, y_m) / scale;

  float head_rad = radians(heading);
  float cog_rad = radians(course);
  mat2 rot_mat_head = mat2( vec2(cos(head_rad), sin(head_rad)), vec2(-sin(head_rad), cos(head_rad)));
  mat2 rot_mat_cog = mat2( vec2(cos(cog_rad), sin(cog_rad)), vec2(-sin(cog_rad), cos(cog_rad)));

  if (heading < 0.0)
    rot_mat_head = rot_mat_cog;

  if (type == 0.0) {
    if (vertex_order == 0.0) {
      pix_pos = pix_pos + rot_mat_head*vec2(-16.0, -16.0);
      v_inner_texcoords = vec2(0.0, 1.0);
    } else if (vertex_order == 1.0) {
      pix_pos = pix_pos + rot_mat_head*vec2(-16.0, 16.0);
      v_inner_texcoords = vec2(0.0, 0.0);
    } else if (vertex_order == 2.0) {
      pix_pos = pix_pos + rot_mat_head*vec2(16.0, 16.0);
      v_inner_texcoords = vec2(1.0, 0.0);
    } else if (vertex_order == 3.0) {
      pix_pos = pix_pos + rot_mat_head*vec2(16.0, -16.0);
      v_inner_texcoords = vec2(1.0, 1.0);
    }
  } else if (type == 1.0 && heading >= 0.0) {
    if (vertex_order == 1.0 || vertex_order == 2.0)
      pix_pos = pix_pos + rot_mat_head * vec2(0.0, -48.0);
    if (vertex_order == 3.0)
      pix_pos = pix_pos + rot_mat_head * vec2(sign(rotation)*8.0, -48.0);
  } else if (type == 2.0) {
    if (vertex_order == 1.0)
      pix_pos = pix_pos + rot_mat_cog * vec2(0.0, -speed);
  }

  v_type = type;
  gl_Position = mvp_matrix  * vec4(pix_pos, 0.0, 1.0);
}
