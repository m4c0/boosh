#include "../glslinc/3d.glsl"

layout(constant_id = 0) const float aspect = 1.0;

const float fov_rad = radians(90);
const float far = 100.0;
const float near = 0.01;

const vec3 up = normalize(vec3(0, 1, 0));

void mvp(vec4 cam, mat4 model, vec3 pos) {
  mat4 proj = projection_matrix(fov_rad, aspect, near, far);
  mat4 view = view_matrix(cam.xyz, radians(cam.w), up);
  gl_Position = vec4(pos.x, -pos.y, pos.z, 1) * model * view * proj;
}
