#pragma leco add_resource "hand.png"
#pragma leco add_resource "hand-attack.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import v;

namespace hand {
  export class model {
    static constexpr const dotz::vec2 neutral_pos { 0.2f };
    static constexpr const float return_speed = 10.0f;
    static constexpr const float follow_speed = 10.0f;
    static constexpr const float theta_speed = 10.0f;
    static constexpr const float move_radius_x = 0.2f;

    struct upc {
      dotz::vec2 pos { neutral_pos };
      dotz::vec2 size { 0.8f };
    } m_pc {};

    voo::one_quad m_quad;
    v::simple_pipeline<upc> m_ppl;
    float m_theta = 0;

  public:
    explicit model(voo::device_and_queue & dq)
      : m_quad { dq.physical_device() }
      , m_ppl { &dq, "hand.png", "hand", {
        .depth_test = false,
        .bindings { m_quad.vertex_input_bind() },
        .attributes { m_quad.vertex_attribute(0) },
      }}
    {}

    void tick(float ms, bool moved) {
      // TODO: make movement speed proportial to player speed
      if (moved) {
        dotz::vec2 target = neutral_pos;
        target.x += move_radius_x * dotz::cos(m_theta);
        target.y += move_radius_x * dotz::abs(dotz::sin(m_theta));

        m_theta += ms * theta_speed / 1000.0f;
        m_pc.pos = dotz::mix(m_pc.pos, target, ms * follow_speed / 1000.0f);
      } else {
        m_theta = 0;
        m_pc.pos = dotz::mix(m_pc.pos, neutral_pos, ms * return_speed / 1000.0f);
      }
    }

    void run(vee::command_buffer cb) {
      m_ppl.cmd_bind(cb, m_pc);
      m_quad.run(cb, 0, 1);
    }
  };
}
