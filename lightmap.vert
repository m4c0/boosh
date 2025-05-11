#version 450

layout(location = 0) in vec2 pos;

layout(location = 0) out vec2 f_pos;

void main() {
  gl_Position = vec4(pos * 2.0 - 1.0, 0, 1);
  f_pos = pos;
}
