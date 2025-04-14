export module faces;
import dotz;
import voo;

export namespace faces {
  struct mdl {
    dotz::vec3 pos;
  };

  struct vtx {
    dotz::vec3 pos;
    dotz::vec2 uv;
    unsigned txt;
  };

  void draw_ceiling(voo::memiter<vtx> & m, int x, int y, float c, unsigned t) {
    float x0 = x;
    float x1 = x + 1;
    float y0 = y;
    float y1 = y + 1;
  
    m += vtx { .pos = { x0, c, y0 }, .uv = { 0, 0 }, .txt = t };
    m += vtx { .pos = { x1, c, y0 }, .uv = { 1, 0 }, .txt = t };
    m += vtx { .pos = { x1, c, y1 }, .uv = { 1, 1 }, .txt = t };
  
    m += vtx { .pos = { x1, c, y1 }, .uv = { 1, 1 }, .txt = t };
    m += vtx { .pos = { x0, c, y1 }, .uv = { 0, 1 }, .txt = t };
    m += vtx { .pos = { x0, c, y0 }, .uv = { 0, 0 }, .txt = t };
  }
  
  void draw_floor(voo::memiter<vtx> & m, int x, int y, float f, unsigned t) {
    float x0 = x;
    float x1 = x + 1;
    float y0 = y;
    float y1 = y + 1;
  
    m += vtx { .pos = { x0, f, y0 }, .uv = { 0, 0 }, .txt = t };
    m += vtx { .pos = { x1, f, y1 }, .uv = { 1, 1 }, .txt = t };
    m += vtx { .pos = { x1, f, y0 }, .uv = { 1, 0 }, .txt = t };
  
    m += vtx { .pos = { x1, f, y1 }, .uv = { 1, 1 }, .txt = t };
    m += vtx { .pos = { x0, f, y0 }, .uv = { 0, 0 }, .txt = t };
    m += vtx { .pos = { x0, f, y1 }, .uv = { 0, 1 }, .txt = t };
  }
  
  void draw_x_wall(voo::memiter<vtx> & m, float x0, float x1, float y, float f, float c, unsigned t) {
    m += vtx { .pos = { x0, f, y }, .uv = { 0, 1 }, .txt = t };
    m += vtx { .pos = { x1, f, y }, .uv = { 1, 1 }, .txt = t };
    m += vtx { .pos = { x1, c, y }, .uv = { 1, 0 }, .txt = t };
  
    m += vtx { .pos = { x1, c, y }, .uv = { 1, 0 }, .txt = t };
    m += vtx { .pos = { x0, c, y }, .uv = { 0, 0 }, .txt = t };
    m += vtx { .pos = { x0, f, y }, .uv = { 0, 1 }, .txt = t };
  }
  
  void draw_y_wall(voo::memiter<vtx> & m, float x, float y0, float y1, float f, float c, unsigned t) {
    m += vtx { .pos = { x, f, y0 }, .uv = { 0, 1 }, .txt = t };
    m += vtx { .pos = { x, f, y1 }, .uv = { 1, 1 }, .txt = t };
    m += vtx { .pos = { x, c, y1 }, .uv = { 1, 0 }, .txt = t };
                                
    m += vtx { .pos = { x, c, y1 }, .uv = { 1, 0 }, .txt = t };
    m += vtx { .pos = { x, c, y0 }, .uv = { 0, 0 }, .txt = t };
    m += vtx { .pos = { x, f, y0 }, .uv = { 0, 1 }, .txt = t };
  }

  void draw_wall(voo::memiter<vtx> & m, float x, float y, float f, float c, unsigned t) {
    draw_x_wall(m, x,     x + 1, y + 1, -1, 1, t);
    draw_x_wall(m, x + 1, x,     y,     -1, 1, t);
    draw_y_wall(m, x,     y,     y + 1, -1, 1, t);
    draw_y_wall(m, x + 1, y + 1, y,     -1, 1, t);
  }
}
