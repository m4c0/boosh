#pragma leco add_shader "model.frag"
#pragma leco add_shader "model.vert"
export module model;
import mapper;
import ppl_with_txt;
import traits;
import v;
import wavefront;

namespace model {
  // Copied from wavefront::vtx because clang explodes on windows otherwise
  struct vtx {
    dotz::vec3 pos;
    dotz::vec2 txt;
    dotz::vec3 nrm;
  };
  export struct mdl {
    dotz::vec3 pos;
    float rot;
    unsigned txt;
  };

  export void create_pipeline() {
    v::g->model_pipeline = ppl_with_txt::create<v::camera>("model", {
      .bindings {
        vee::vertex_input_bind(sizeof(vtx)),
        vee::vertex_input_bind_per_instance(sizeof(mdl)),
      },
      .attributes {
        vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
        vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::txt)),
        vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::nrm)),
        vee::vertex_attribute_vec4(1, traits::offset_of(&mdl::pos)),
        vee::vertex_attribute_uint(1, traits::offset_of(&mdl::txt)),
      },
    });
  }
  export void bind(vee::command_buffer cb) {
    ppl_with_txt::cmd_bind(cb, &v::g->model_pipeline);
    vee::cmd_push_vertex_constants(cb, *v::g->model_pipeline.layout, &v::g->camera);
  }

  export template<typename T> T create(int x, int y, int id, mapper::tiledef);
  export template<typename T> mdl convert(T);
  export template<typename T> void remove(int id, T);

  export class batch {
    static constexpr const auto max_models = 128;

    voo::bound_buffer m_buf;
    voo::bound_buffer m_mdl;
    unsigned m_vcount;
    unsigned m_icount;

  protected:
    virtual void load(voo::memiter<mdl> & m) = 0;

    auto memiter() { return voo::memiter<mdl> { *m_mdl.memory }; }

  public:
    explicit batch(jute::view model)
      : m_mdl { voo::bound_buffer::create_from_host(v::g->pd, max_models * sizeof(mdl), vee::buffer_usage::vertex_buffer) }
    {
      auto [buf, count] = wavefront::load_model(v::g->pd, model);
      m_buf = traits::move(buf);
      m_vcount = count;
    }

    void setup_copy(vee::command_buffer cb) {
      {
        voo::memiter<mdl> m { *m_mdl.memory, &m_icount };
        load(m);
      }
    }

    void draw(vee::command_buffer cb) {
      vee::cmd_bind_vertex_buffers(cb, 0, *m_buf.buffer);
      vee::cmd_bind_vertex_buffers(cb, 1, *m_mdl.buffer);
      vee::cmd_draw(cb, m_vcount, m_icount);
    }
  };

  export template<typename T> class list : public batch {
    hai::varray<T> m_list { 128 };

    void load(voo::memiter<::model::mdl> & m) override {
      for (auto p : m_list) m += ::model::convert(p);
    }

  protected:
    [[nodiscard]] constexpr auto & data() { return m_list; }

  public:
    using batch::batch;

    void remove(int id) {
      model::remove(id, m_list[id]);
      m_list[id] = {};
    }

    void clear() {
      for (auto id = 0; id < m_list.size(); id++) remove(id);
      m_list.truncate(0);
    }

    void load_map(const mapper::tilemap * map) {
      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity != T::entity) return;
        m_list.push_back(create<T>(x, y, m_list.size(), d));
      });
    }
  };
}
