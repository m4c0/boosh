export module faces;
import dotz;
import traits;
import voo;

export namespace faces {
  struct vtx {
    dotz::vec3 pos;
    dotz::vec2 uv;
  };
  struct inst {
    dotz::vec3 pos;
    unsigned txt;
  };

  inline const auto bindings() {
    return decltype(vee::gr_pipeline_params::bindings) {
      vee::vertex_input_bind(sizeof(vtx)),
      vee::vertex_input_bind_per_instance(sizeof(inst)),
    };
  }
  inline const auto attributes() {
    return decltype(vee::gr_pipeline_params::attributes) {
      vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
      vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::uv)),
      vee::vertex_attribute_vec3(1, traits::offset_of(&inst::pos)),
      vee::vertex_attribute_uint(1, traits::offset_of(&inst::txt)),
    };
  }

  class instanced {
    static constexpr const auto max_inst = 256 * 256;

    voo::h2l_buffer m_vtx;
    voo::h2l_buffer m_inst;
    unsigned m_v_count;
    unsigned m_i_count;

  protected:
    [[nodiscard]] auto map_vertex() {
      return voo::memiter<vtx> { m_vtx.host_memory() };
    }

  public:
    explicit instanced(vee::physical_device pd, unsigned v)
      : m_vtx { pd, static_cast<unsigned>(sizeof(vtx) * v) }
      , m_inst { pd, sizeof(inst) * max_inst }
      , m_v_count { v }
    {}

    [[nodiscard]] auto map() {
      return voo::memiter<inst> { m_inst.host_memory(), &m_i_count };
    }
    [[nodiscard]] auto remap() {
      return voo::memiter<inst> { m_inst.host_memory() };
    }

    void setup_copy(vee::command_buffer cb) {
      m_vtx.setup_copy(cb);
      m_inst.setup_copy(cb);
    }

    void draw(vee::command_buffer cb) {
      vee::cmd_bind_vertex_buffers(cb, 0, m_vtx.local_buffer());
      vee::cmd_bind_vertex_buffers(cb, 1, m_inst.local_buffer());
      vee::cmd_draw(cb, m_v_count, m_i_count);
    }
  };

  class ceiling : public instanced {
  public:
    explicit ceiling(vee::physical_device pd) : instanced { pd, 6 } {
      auto m = map_vertex();

      m += vtx { .pos = { 0, 1, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 1, 1, 0 }, .uv = { 1, 0 } };
      m += vtx { .pos = { 1, 1, 1 }, .uv = { 1, 1 } };
    
      m += vtx { .pos = { 1, 1, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 0, 1, 1 }, .uv = { 0, 1 } };
      m += vtx { .pos = { 0, 1, 0 }, .uv = { 0, 0 } };
    }
  };

  class floor : public instanced {
  public:
    explicit floor(vee::physical_device pd) : instanced { pd, 6 } {
      auto m = map_vertex();

      m += vtx { .pos = { 0, -1, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 1, -1, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 1, -1, 0 }, .uv = { 1, 0 } };
    
      m += vtx { .pos = { 1, -1, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 0, -1, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 0, -1, 1 }, .uv = { 0, 1 } };
    }
  };

  class wall : public instanced {
    static void x_wall(voo::memiter<vtx> & m, float x0, float x1, float y) {
      m += vtx { .pos = { x0, -1.f, y }, .uv = { 0, 1 } };
      m += vtx { .pos = { x1, -1.f, y }, .uv = { 1, 1 } };
      m += vtx { .pos = { x1,  1.f, y }, .uv = { 1, 0 } };
    
      m += vtx { .pos = { x1,  1.f, y }, .uv = { 1, 0 } };
      m += vtx { .pos = { x0,  1.f, y }, .uv = { 0, 0 } };
      m += vtx { .pos = { x0, -1.f, y }, .uv = { 0, 1 } };
    }
    static void y_wall(voo::memiter<vtx> & m, float x, float y0, float y1) {
      m += vtx { .pos = { x, -1.f, y0 }, .uv = { 0, 1 } };
      m += vtx { .pos = { x, -1.f, y1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { x,  1.f, y1 }, .uv = { 1, 0 } };

      m += vtx { .pos = { x,  1.f, y1 }, .uv = { 1, 0 } };
      m += vtx { .pos = { x,  1.f, y0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { x, -1.f, y0 }, .uv = { 0, 1 } };
    }

  public:
    explicit wall(vee::physical_device pd) : instanced { pd, 24 } {
      auto m = map_vertex();
      x_wall(m, 0, 1, 1);
      x_wall(m, 1, 0, 0);
      y_wall(m, 0, 0, 1);
      y_wall(m, 1, 1, 0);
    }
  };
}
