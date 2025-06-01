#version 450
#extension GL_GOOGLE_include_directive : require
#include "mvp.glsl"

layout(push_constant) uniform upc {
  vec4 cam;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec2 nrm;
layout(location = 3) in vec4 i_pos;
layout(location = 4) in uint i_txt;

layout(location = 0) out vec2 f_uv;
layout(location = 1) out vec3 f_pos;
layout(location = 2) out uint f_txt;

void main() {
  vec3 p = pos + i_pos.xyz;
  mvp(cam, mat4(1), p);
  f_uv = uv;
  f_pos = p;
  f_txt = i_txt;
}
