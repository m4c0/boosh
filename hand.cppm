#pragma leco add_resource "hand.png"
#pragma leco add_resource "hand-attack.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import v;

namespace hand {
  enum class states {
    walking,
    to_attack,
    attacking,
    to_walk,
  } state;

  export void attack() {
    // TODO: enable attack when hand is returning
    if (state != states::walking) return;
    state = states::to_attack;
  }

  export class model {
    static constexpr const dotz::vec2 neutral_pos { 0.2f };
    static constexpr const dotz::vec2 holster_pos = neutral_pos + dotz::vec2 { 0.0f, 1.0f };
    static constexpr const dotz::vec2 attack_start_pos { -1.8f, 1.0f };
    static constexpr const dotz::vec2 attack_end_pos { -0.8f, -0.5f };

    static constexpr const dotz::vec2 neutral_size { 0.8f };
    static constexpr const dotz::vec2 attack_size { 1.5f };

    static constexpr const float return_speed = 10.0f;
    static constexpr const float follow_speed = 10.0f;
    static constexpr const float holster_speed = 20.0f;
    static constexpr const float attack_speed = 20.0f;
    static constexpr const float theta_speed = 10.0f;
    static constexpr const float move_radius_x = 0.2f;

    struct upc {
      dotz::vec2 pos { neutral_pos };
      dotz::vec2 size { neutral_size };
    } m_pc {};

    voo::one_quad m_quad;
    v::simple_pipeline<upc> m_ppl;
    voo::host_buffer_for_image m_atk_img;
    float m_theta = 0;

  public:
    explicit model(voo::device_and_queue & dq)
      : m_quad { dq.physical_device() }
      , m_ppl { &dq, "hand.png", "hand", {
        .depth_test = false,
        .bindings { m_quad.vertex_input_bind() },
        .attributes { m_quad.vertex_attribute(0) },
      }}
      , m_atk_img { voo::load_image_file_as_buffer("hand-attack.png", dq.physical_device()) }
    {}

    void tick(float ms, bool moved) {
      // TODO: improve easy in/out of attack
      if (state == states::to_attack) {
        m_pc.pos = dotz::mix(m_pc.pos, holster_pos, ms * holster_speed / 1000.0f);
        if (dotz::length(m_pc.pos - holster_pos) < 0.01) {
          state = states::attacking;
          m_pc.pos = attack_start_pos;
          m_pc.size = attack_size;
          m_ppl.copy_image(m_atk_img);
        }
        return;
      } else if (state == states::attacking) {
        m_pc.pos = dotz::mix(m_pc.pos, attack_end_pos, ms * attack_speed / 1000.0f);
        if (dotz::length(m_pc.pos - attack_end_pos) < 0.01) {
          state = states::to_walk;
        }
        return;
      } else if (state == states::to_walk) {
        m_pc.pos = dotz::mix(m_pc.pos, attack_start_pos, ms * attack_speed / 1000.0f);
        if (dotz::length(m_pc.pos - attack_start_pos) < 0.01) {
          state = states::walking;
          m_pc.pos = holster_pos;
          m_pc.size = neutral_size;
          m_ppl.copy_image();
        }
        return;
      }

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
