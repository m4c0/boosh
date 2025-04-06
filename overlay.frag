#version 450

layout(location = 0) in vec2 pos;
layout(location = 0) out vec4 colour;

void main() {
  float d = length(pos);
  d = clamp(d, 0, 1);
  colour = vec4(d, 0, 0, d);
}
