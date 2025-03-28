#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 colour;

void main() {
  vec2 p = uv * vec2(3, 6);
  if (fract(p.y * 0.5) > 0.5) p.x += 0.5;
  p = fract(p);
  colour = vec4(p, 0, 1);
}
