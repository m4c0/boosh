#pragma leco add_shader "overlay.vert"
#pragma leco add_shader "overlay.frag"
export module overlay;
import v;

namespace overlay {
  export class model {
    vee::pipeline_layout m_pl = vee::create_pipeline_layout({
      vee::fragment_push_constant_range<dotz::vec4>()
    });
    voo::one_quad m_quad;
    vee::gr_pipeline m_pipeline;

  public:
    explicit model(vee::image_view::type lgm_iv)
      : m_quad { v::g->pd }
      , m_pipeline {
        vee::create_graphics_pipeline({
          .pipeline_layout = *m_pl,
          .render_pass = v::g->dq->render_pass(),
          .depth_test = false,
          .shaders {
            voo::shader("overlay.vert.spv").pipeline_vert_stage(),
            voo::shader("overlay.frag.spv").pipeline_frag_stage(),
          },
          .bindings { m_quad.vertex_input_bind() },
          .attributes { m_quad.vertex_attribute(0) },
        })
      }
    {}

    void run(vee::command_buffer cb, dotz::vec4 olay) {
      vee::cmd_push_fragment_constants(cb, *m_pl, &olay);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
      m_quad.run(cb, 0, 1);
    }
  };
}
