#version 450

layout(set = 0, binding = 0) uniform sampler2D txt[16];

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 colour;

void main() {
  colour = vec4(texture(txt[0], uv).rgb, 1);
}
