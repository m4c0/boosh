#pragma leco add_shader "model.frag"
#pragma leco add_shader "model.vert"
export module model;
import collision;
import faces;
import hai;
import traits;
import v;
import wavefront;

namespace model {
  export class batch {
    struct upc {
      dotz::vec3 cam;
      float angle;
    };

    // Copied from wavefront::vtx because clang explodes on windows otherwise
    struct vtx {
      dotz::vec3 pos;
      dotz::vec2 txt;
      dotz::vec3 nrm;
    };

    static constexpr const auto max_models = 128;

    v::ppl_with_txt<upc> m_ppl;
    voo::h2l_buffer m_buf;
    voo::h2l_buffer m_mdl;
    unsigned m_vcount;
    unsigned m_icount;

  protected:
    struct mdl {
      dotz::vec3 pos;
      float rot;
    };

    virtual void load(voo::memiter<mdl> & m) = 0;

    auto memiter() { return voo::memiter<mdl> { m_mdl.host_memory() }; }

  public:
    batch(voo::device_and_queue & dq, vee::image_view::type lgm_iv, jute::view model, const char * txt)
      : m_ppl { &dq, lgm_iv, txt, "model", {
        .bindings {
          vee::vertex_input_bind_per_instance(sizeof(mdl)),
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec4(0, traits::offset_of(&mdl::pos)),
          vee::vertex_attribute_vec3(1, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(1, traits::offset_of(&vtx::txt)),
          vee::vertex_attribute_vec3(1, traits::offset_of(&vtx::nrm)),
        },
      }}
      , m_mdl { v::g->pd, max_models * sizeof(mdl) }
    {
      auto [buf, count] = wavefront::load_model(dq.physical_device(), model);
      m_buf = traits::move(buf);
      m_vcount = count;
    }

    void setup_copy(vee::command_buffer cb) {
      {
        voo::memiter<mdl> m { m_mdl.host_memory(), &m_icount };
        load(m);
      }
      m_buf.setup_copy(cb);
      m_mdl.setup_copy(cb);
    }
    void copy_models(vee::command_buffer cb) {
      m_mdl.setup_copy(cb);
    }

    void draw(vee::command_buffer cb, dotz::vec3 cam, float angle) {
      m_ppl.cmd_bind(cb, { cam, angle });
      vee::cmd_bind_vertex_buffers(cb, 0, m_mdl.local_buffer());
      vee::cmd_bind_vertex_buffers(cb, 1, m_buf.local_buffer());
      vee::cmd_draw(cb, m_vcount, m_icount);
    }
  };
}
