#version 450
#extension GL_GOOGLE_include_directive : require
#include "mvp.glsl"

layout(push_constant) uniform upc {
  vec4 cam;
};

layout(location = 0) in vec3 mdl;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec2 txt;
layout(location = 3) in vec3 nrm;

layout(location = 0) out vec2 f_txt;
layout(location = 1) out vec3 f_nrm;

void main() {
  mat4 model = translation(mdl);

  mvp(cam, model, pos);
  f_txt = txt;
  f_nrm = nrm;
}
