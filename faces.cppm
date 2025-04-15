export module faces;
import dotz;
import traits;
import voo;

export namespace faces {
  struct vtx {
    dotz::vec3 pos;
    dotz::vec2 uv;
    unsigned txt;
  };
  struct inst {
    dotz::vec3 pos;
    unsigned txt;
  };

  constexpr const auto bindings() {
    return decltype(vee::gr_pipeline_params::bindings) {
      vee::vertex_input_bind(sizeof(vtx)),
      vee::vertex_input_bind_per_instance(sizeof(inst)),
    };
  }
  constexpr const auto attributes() {
    return decltype(vee::gr_pipeline_params::attributes) {
      vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
      vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::uv)),
      vee::vertex_attribute_uint(0, traits::offset_of(&vtx::txt)),
      vee::vertex_attribute_vec3(1, traits::offset_of(&inst::pos)),
      vee::vertex_attribute_uint(1, traits::offset_of(&inst::txt)),
    };
  }

  class ceiling {
    static constexpr const auto max_inst = 256 * 256;

    voo::h2l_buffer m_vtx;
    voo::h2l_buffer m_inst;
    unsigned m_count;

  public:
    explicit ceiling(vee::physical_device pd)
      : m_vtx { pd, sizeof(vtx) * 6 }
      , m_inst { pd, sizeof(inst) * max_inst }
    {
      voo::memiter<vtx> m { m_vtx.host_memory() };
      m += vtx { .pos = { 0, 0, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 1, 0, 0 }, .uv = { 1, 0 } };
      m += vtx { .pos = { 1, 0, 1 }, .uv = { 1, 1 } };
    
      m += vtx { .pos = { 1, 0, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 0, 0, 1 }, .uv = { 0, 1 } };
      m += vtx { .pos = { 0, 0, 0 }, .uv = { 0, 0 } };
    }

    [[nodiscard]] auto map() {
      return voo::memiter<inst> { m_inst.host_memory(), &m_count };
    }

    void setup_copy(vee::command_buffer cb) {
      m_vtx.setup_copy(cb);
      m_inst.setup_copy(cb);
    }

    void draw(vee::command_buffer cb) {
      vee::cmd_bind_vertex_buffers(cb, 0, m_vtx.local_buffer());
      vee::cmd_bind_vertex_buffers(cb, 1, m_inst.local_buffer());
      vee::cmd_draw(cb, 6, m_count);
    }
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
