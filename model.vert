#version 450
#extension GL_GOOGLE_include_directive : require
#include "mvp.glsl"

layout(push_constant) uniform upc {
  vec4 cam;
};

layout(location = 0) in vec4 mdl;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec2 txt;
layout(location = 3) in vec3 nrm;

layout(location = 0) out vec2 f_uv;
layout(location = 1) out vec3 f_pos;
layout(location = 2) out uint f_txt;

void main() {
  mat4 rot = mat4(
    cos(mdl.w), 0, sin(mdl.w), 0,
    0, 1, 0, 0,
    -sin(mdl.w), 0, cos(mdl.w), 0,
    0, 0, 0, 1
  );
  mat4 model = rot * translation(mdl.xyz);
  vec4 p = vec4(pos, 1) * model;

  mvp(cam, model, pos);
  f_uv = txt;
  f_pos = p.xyz;
}
