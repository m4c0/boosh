#version 450

layout(set = 0, binding = 0) uniform sampler2D textures[8];
layout(set = 0, binding = 1) uniform sampler2D lightmap;

layout(location = 0) in vec2 uv;
layout(location = 1) in flat uint txt;

layout(location = 0) out vec4 colour;

void main() {
  colour = vec4(texture(textures[txt], uv).rgb, 1);
}
