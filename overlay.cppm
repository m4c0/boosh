#pragma leco add_shader "overlay.vert"
#pragma leco add_shader "overlay.frag"
export module overlay;
import v;

namespace overlay {
  export class model {
    vee::pipeline_layout m_pl = vee::create_pipeline_layout({
      vee::fragment_push_constant_range<dotz::vec4>()
    });
    voo::one_quad_render m_oqr;

  public:
    explicit model(voo::device_and_queue & dq)
      : m_oqr { "overlay", &dq, *m_pl }
    {}

    void run(vee::command_buffer cb, dotz::vec4 olay) {
      // TODO: remove depth test
      vee::cmd_push_fragment_constants(cb, *m_pl, &olay);
      m_oqr.run(cb);
    }
  };
}
