#pragma leco add_shader "bullet.frag"
#pragma leco add_shader "bullet.vert"
#pragma leco add_resource "bullet.obj"
export module bullet;
import traits;
import voo;
import wavefront;

namespace bullet {
  struct upc {
    float aspect;
  };

  using vtx = wavefront::vtx;

  export class model {
    vee::pipeline_layout m_pl = vee::create_pipeline_layout({ vee::vertex_push_constant_range<upc>() });
    vee::gr_pipeline m_gp;
    voo::h2l_buffer m_buf;
    unsigned m_vcount;
  public:
    explicit model(voo::device_and_queue & dq)
      : m_gp { vee::create_graphics_pipeline({
        .pipeline_layout = *m_pl,
        .render_pass = dq.render_pass(),
        .shaders {
          voo::shader("bullet.vert.spv").pipeline_vert_stage(),
          voo::shader("bullet.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::txt)),
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::nrm)),
        },
      }) }
    {
      auto [buf, count] = wavefront::load_model(dq.physical_device(), "bullet.obj");
      m_buf = traits::move(buf);
      m_vcount = count;
    }

    void setup_copy(vee::command_buffer cb) {
      m_buf.setup_copy(cb);
    }

    void draw(vee::command_buffer cb) {
      upc pc {};
      vee::cmd_bind_vertex_buffers(cb, 0, m_buf.local_buffer());
      vee::cmd_push_vertex_constants(cb, *m_pl, &pc);
      vee::cmd_bind_gr_pipeline(cb, *m_gp);
      vee::cmd_draw(cb, m_vcount);
    }
  };
}
