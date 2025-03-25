#version 450

layout(location = 0) in vec4 pos;

const float fov_rad = radians(90);
const float aspect = 1;

const float f = 1 / tan(fov_rad / 2);
const float far = 1000.0;
const float near = 0.01;

const mat4 proj = mat4(
  f / aspect, 0.0, 0.0, 0.0,
  0.0, f, 0.0, 0.0,
  0.0, 0.0, (far + near) / (near - far), (2.0 * far * near) / (near - far),
  0.0, 0.0, -1.0, 0.0
);

vec3 cam = vec3(0.0, 0.0, 5.0);
float angle = radians(45);
void main() {
  float ca = cos(angle);
  float sa = sin(angle);
  vec3 fwd = (mat4(
    +ca, 0.0, +sa, 0.0,
    0.0, 1.0, 0.0, 0.0,
    -sa, 0.0, +ca, 0.0,
    0.0, 0.0, 0.0, 1.0
  ) * vec4(0.0, 0.0, -1.0, 0.0)).xyz;

  vec3 up = vec3(0, 1, 0);
  vec3 right = normalize(cross(up, fwd));

  mat4 view = transpose(mat4(
    vec4(right, 0),
    vec4(up, 0),
    vec4(fwd, 0),
    vec4(-dot(right, cam), -dot(up, cam), -dot(fwd, cam), 1)
  ));

  gl_Position = proj * view * pos;
}
