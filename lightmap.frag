#version 450

layout(set = 0, binding = 0) uniform sampler2D txt;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

void main() {
  colour = vec4(texture(txt, f_pos).rgb, 1);
}
