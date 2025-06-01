#version 450

layout(constant_id = 99) const uint txt_count = 8;

layout(set = 0, binding = 0) uniform sampler2D lightmap;
layout(set = 0, binding = 1) uniform sampler2D textures[txt_count];

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 pos;
layout(location = 2) in flat uint txt;

layout(location = 0) out vec4 colour;

const float lmap_sz = 32.0;

void main() {
  vec3 c = texture(textures[txt], uv).rgb;
  float l = texture(lightmap, pos.xz / lmap_sz).b;
  colour = vec4(c * l, 1);
}
