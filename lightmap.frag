#version 450

layout(push_constant) uniform upc { vec2 extent; };

layout(set = 0, binding = 0) uniform sampler2D txt;

layout(location = 0) in vec2 f_pos;

layout(location = 0) out vec4 colour;

void main() {
  vec4 c = texture(txt, f_pos);
  vec4 cu = texture(txt, f_pos - vec2(0, 1.0 / extent.y));
  vec4 cd = texture(txt, f_pos + vec2(0, 1.0 / extent.y));
  vec4 cl = texture(txt, f_pos - vec2(1.0 / extent.x, 0));
  vec4 cr = texture(txt, f_pos + vec2(1.0 / extent.x, 0));

  float b = c.r;
  b = max(b, max(cu.r, cu.b));
  b = max(b, max(cd.r, cd.b));
  b = max(b, max(cl.r, cl.b));
  b = max(b, max(cr.r, cr.b));
  b = clamp(b, 0, 1);
  //b = b * 0.9 * c.g;
  colour = vec4(c.rg, b, 1);
}
