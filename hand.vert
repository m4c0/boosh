#version 450

layout(push_constant) uniform upc {
  vec2 pos;
  vec2 size;
};

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 frag_coord;

void main() {
  vec2 p = size * position + pos;
  gl_Position = vec4(p, 0, 1);
  frag_coord = position;
}
