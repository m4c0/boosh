#version 450

layout(set = 0, binding = 0) uniform sampler2D txt;
layout(set = 0, binding = 1) uniform sampler2D lightmap;

layout(location = 0) out vec4 colour;

layout(location = 0) in vec2 f_txt;
layout(location = 1) in vec3 f_pos;

const float lmap_sz = 32.0;

void main() {
  vec3 c = texture(txt, f_txt).rgb;
  float l = texture(lightmap, f_pos.xz / lmap_sz).b;
  colour = vec4(c * l, 1);
}
