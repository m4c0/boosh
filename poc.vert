#version 450
#extension GL_GOOGLE_include_directive : require
#include "../glslinc/3d.glsl"

layout(push_constant) uniform upc {
  vec4 cam;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in uint txt;

layout(location = 0) out vec2 f_uv;
layout(location = 1) out uint f_txt;

const float fov_rad = radians(90);
const float aspect = 1;
const float far = 100.0;
const float near = 0.01;

const vec3 up = normalize(vec3(0, 1, 0));

void main() {
  mat4 proj = projection_matrix(fov_rad, aspect, near, far);
  mat4 view = view_matrix(cam.xyz, radians(cam.w), up);
  gl_Position = vec4(pos.x, -pos.y, pos.z, 1) * view * proj;
  f_uv = uv;
  f_txt = txt;
}
