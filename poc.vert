#version 450

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

const float f = 1 / atan(fov_rad / 2);
const float far = 100.0;
const float near = 0.01;

const float p22 = (far + near) / (near - far);
const float p32 = (2.0 * far * near) / (near - far);
const mat4 proj = mat4(
  f / aspect, 0.0, 0.0, 0.0,
  0.0, f, 0.0, 0.0,
  0.0, 0.0, p22, p32,
  0.0, 0.0, -1, 0.0
);

const vec3 up = normalize(vec3(0, 1, 0));
mat4 view() {
  float a = radians(cam.w);
  vec3 f = normalize(vec3(sin(a), 0, cos(a)));
  vec3 s = normalize(cross(f, up));
  vec3 u = cross(s, f);
  mat4 trn = mat4(
    vec4(1, 0, 0, -cam.x),
    vec4(0, 1, 0, -cam.y),
    vec4(0, 0, 1, -cam.z),
    vec4(0, 0, 0, 1)
  );
  mat4 rot = mat4(
    vec4( s, 0),
    vec4( u, 0),
    vec4(-f, 0),
    vec4(0, 0, 0, 1)
  );
  return trn * rot;
}

void main() {
  gl_Position = vec4(pos.x, -pos.y, pos.z, 1) * view() * proj;
  f_uv = uv;
  f_txt = txt;
}
