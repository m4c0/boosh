#pragma leco add_shader "overlay.vert"
#pragma leco add_shader "overlay.frag"
export module overlay;
import shaders;
import v;

namespace overlay {
  export class model {
    vee::pipeline_layout m_pl = vee::create_pipeline_layout({
      vee::fragment_push_constant_range<dotz::vec4>()
    });
    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;

    dotz::vec4 m_value {};

  public:
    explicit model()
      : m_quad { v::g->pd }
      , m_pipeline {
        vee::create_graphics_pipeline({
          .pipeline_layout = *m_pl,
          .render_pass = *v::g->rp,
          .depth_test = false,
          .shaders {
            shaders::get("overlay.vert.spv").pipeline_vert_stage(),
            shaders::get("overlay.frag.spv").pipeline_frag_stage(),
          },
          .bindings { m_quad.vertex_input_bind() },
          .attributes { m_quad.vertex_attribute(0) },
        })
      }
    {}

    constexpr void set(dotz::vec4 v) { m_value = v; }

    void tick(float millis) {
      // TODO: make this dependent on time
      m_value = m_value * 0.9;
    }

    void run(vee::command_buffer cb) {
      vee::cmd_push_fragment_constants(cb, *m_pl, &m_value);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      m_quad.run(cb, 0, 1);
    }
  };
}
