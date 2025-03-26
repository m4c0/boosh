#version 450

layout(location = 0) in vec4 pos;

const float fov_rad = radians(90);
const float aspect = 1;

const float f = 1 / atan(fov_rad / 2);
const float far = 100.0;
const float near = 0.1;

const float p22 = (far + near) / (near - far);
const float p32 = (2.0 * far * near) / (near - far);
const mat4 proj = mat4(
  f / aspect, 0.0, 0.0, 0.0,
  0.0, f, 0.0, 0.0,
  0.0, 0.0, p22, p32,
  0.0, 0.0, -1, 0.0
);

vec3 cam = vec3(0.0, 0.0, 5.0);
vec3 tgt = vec3(0, 0, -1);
const vec3 up = normalize(vec3(0, 1, 0));
void main() {
  vec3 f = normalize(tgt);
  vec3 s = normalize(cross(f, up));
  vec3 u = cross(s, f);
  mat4 view = mat4(
    vec4( s, -cam.x),
    vec4( u, -cam.y),
    vec4(-f, -cam.z),
    vec4(0, 0, 0, 1)
  );

  gl_Position = pos * view * proj;
}
