#version 450

layout(set = 0, binding = 0) uniform sampler2D txt;

layout(location = 0) out vec4 colour;

layout(location = 0) in vec2 f_txt;
layout(location = 1) in vec3 f_nrm;

void main() {
  colour = vec4(texture(txt, f_txt).rgb, 1.0);
}
