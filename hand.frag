#version 450

layout(constant_id = 99) const uint txt_count = 8;

layout(set = 0, binding = 0) uniform sampler2D lightmap;
layout(set = 0, binding = 1) uniform sampler2D textures[txt_count];

layout(location = 0) in vec2 frag_coord;
layout(location = 1) in vec2 frag_cam;
layout(location = 2) in flat uint f_txt;

layout(location = 0) out vec4 frag_colour;

const float lmap_sz = 32.0;

void main() {
  vec2 p = frag_coord * 0.5 + 0.5;
  vec4 c = texture(textures[f_txt], frag_coord);
  float l = texture(lightmap, frag_cam / lmap_sz).b;
  frag_colour = c * vec4(l, l, l, 1);
}
