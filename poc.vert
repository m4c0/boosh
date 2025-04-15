#version 450
#extension GL_GOOGLE_include_directive : require
#include "mvp.glsl"

layout(push_constant) uniform upc {
  vec4 cam;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 i_pos;
layout(location = 3) in uint i_txt;

layout(location = 0) out vec2 f_uv;
layout(location = 1) out uint f_txt;

void main() {
  mvp(cam, mat4(1), pos + i_pos);
  f_uv = uv;
  f_txt = i_txt;
}
