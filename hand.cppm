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

    voo::one_quad m_quad;
    v::simple_pipeline<upc> m_ppl;

  public:
    explicit model(const voo::device_and_queue & dq)
      : m_quad { dq.physical_device() }
      , m_ppl { &dq, "hand.png", "hand", {
        .bindings { m_quad.vertex_input_bind() },
        .attributes { m_quad.vertex_attribute(0) },
      }}
    {}

    void setup_copy(vee::command_buffer cb) {
      m_ppl.setup_copy(cb);
    }

    void run(vee::command_buffer cb) {
      m_ppl.cmd_bind(cb, m_pc);
      m_quad.run(cb, 0, 1);
    }
  };
}
