#version 450

layout(location = 0) in vec2 pos;
layout(location = 0) out vec4 c;

void main() {
  c = vec4(pos, 0, 1);
}
