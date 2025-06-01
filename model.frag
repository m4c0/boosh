#version 450

layout(constant_id = 99) const uint txt_count = 8;

layout(set = 0, binding = 0) uniform sampler2D lightmap;
layout(set = 0, binding = 1) uniform sampler2D textures[txt_count];

layout(location = 0) in vec2 f_uv;
layout(location = 1) in vec3 f_pos;
layout(location = 2) in flat uint f_txt;

layout(location = 0) out vec4 colour;

const float lmap_sz = 32.0;

void main() {
  vec3 c = texture(textures[f_txt], f_uv).rgb;
  float l = texture(lightmap, f_pos.xz / lmap_sz).b;
  colour = vec4(c * l, 1);
}
