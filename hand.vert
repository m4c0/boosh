#version 450
#extension GL_GOOGLE_include_directive : require
#include "mvp.glsl"

layout(push_constant) uniform upc {
  vec2 pos;
  vec2 size;
  vec2 cam;
};

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 frag_coord;
layout(location = 1) out vec2 frag_cam;
layout(location = 2) out uint f_txt;

void main() {
  vec2 p = vec2(1.0 / aspect, 1) * size * position + pos;
  gl_Position = vec4(p, 0, 1);
  frag_coord = position;
  frag_cam = cam;
  f_txt = 0;
}
