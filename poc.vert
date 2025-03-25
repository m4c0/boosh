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

void main() {
  gl_Position = proj * pos;
}
