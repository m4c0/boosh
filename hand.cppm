#pragma leco add_resource "hand.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import v;

namespace hand {
  export class model {
    struct upc {
      dotz::vec2 pos { 0.2f };
      dotz::vec2 size { 0.8f };
    } m_pc {};

    v::x<upc> m_x;
    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;

  public:
    explicit model(const voo::device_and_queue & dq)
      : m_x { &dq, "hand.png" }
      , m_quad { dq.physical_device() }
      , m_pipeline {
        vee::create_graphics_pipeline({
          .pipeline_layout = *m_x.m_pl,
          .render_pass = dq.render_pass(),
          .shaders {
            voo::shader("hand.vert.spv").pipeline_vert_stage(),
            voo::shader("hand.frag.spv").pipeline_frag_stage(),
          },
          .bindings { m_quad.vertex_input_bind() },
          .attributes { m_quad.vertex_attribute(0) },
        })
      }
    {}

    void setup_copy(vee::command_buffer cb) {
      m_x.setup_copy(cb);
    }

    void run(vee::command_buffer cb) {
      vee::cmd_bind_descriptor_set(cb, *m_x.m_pl, 0, m_x.m_ds.descriptor_set());
      vee::cmd_push_vertex_constants(cb, *m_x.m_pl, &m_pc);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      m_quad.run(cb, 0, 1);
    }
  };
}
