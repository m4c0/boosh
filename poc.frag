#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 colour;

void main() {
  vec2 p = uv * vec2(3, 6);
  if (fract(p.y * 0.5) > 0.5) p.x += 0.5;
  p = fract(p);

  vec2 brick = step(p, vec2(0.9));
  const vec3 c0 = vec3(0.5, 0.5, 0.1);
  const vec3 c1 = vec3(0.6, 0.2, 0.1);
  vec3 c = mix(c0, c1, brick.x * brick.y);

  colour = vec4(c, 1);
}
