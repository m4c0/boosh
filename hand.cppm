#pragma leco add_resource "hand.png"
#pragma leco add_resource "hand-attack.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import mtx;
import silog;
import sith;
import v;

namespace hand {
  struct upc {
    dotz::vec2 pos {};
    dotz::vec2 size {};
  };
}

namespace hand::images {
  enum e {
    NIL,
    HAND,
    PUNCH,
    MAX,
  };
  struct t {
    e id;
    const char * filename;
  };
  constexpr const t all[MAX] = {
    { NIL                      },
    { HAND,  "hand.png"        },
    { PUNCH, "hand-attack.png" },
  };
  static_assert([]{
    for (auto i = 0; i < MAX; i++) if (all[i].id != i) return false;
    return true;
  }());
}
namespace hand::anims {
  static constexpr const dotz::vec2 hand_neutral_pos { 0.2f };
  static constexpr const dotz::vec2 hand_neutral_size { 0.8f };
  static constexpr const dotz::vec2 hand_attack_size { 1.5f };
  static constexpr const dotz::vec2 hand_attack_start_pos { -1.8f, 1.0f };
  static constexpr const dotz::vec2 hand_attack_end_pos { -0.8f, -0.5f };

  static bool bob(upc * pc, float t, float spd) {
    if (spd == 0) {
      pc->pos = hand_neutral_pos;
      pc->size = hand_neutral_size;
      return true;
    }
    float tt = t * 10.0 / 1000.0;
    pc->pos.x = hand_neutral_pos.x + 0.2f * dotz::cos(tt);
    pc->pos.y = hand_neutral_pos.y + 0.2f * dotz::abs(dotz::sin(tt));
    pc->size = hand_neutral_size;
    return true;
  }
  static bool hand_holster(upc * pc, float t, float spd) {
    pc->pos.x = hand_neutral_pos.x;
    pc->pos.y = hand_neutral_pos.y + t / 200.0f;
    pc->size = hand_neutral_size;
    return t < 200;
  }
  static bool punch_go(upc * pc, float t, float spd) {
    pc->pos = dotz::mix(hand_attack_start_pos, hand_attack_end_pos, t / 200.0f);
    pc->size = hand_attack_size;
    return t < 200;
  }
  static bool punch_back(upc * pc, float t, float spd) {
    pc->pos = dotz::mix(hand_attack_end_pos, hand_attack_start_pos, t / 200.0f);
    pc->size = hand_attack_size;
    return t < 200;
  }
  static bool hand_up(upc * pc, float t, float spd) {
    pc->pos.x = hand_neutral_pos.x;
    pc->pos.y = hand_neutral_pos.y + 1.0 - t / 200.0f;
    pc->size = hand_neutral_size;
    return t < 200;
  }
}
namespace hand::stts {
  static constexpr const auto T = true;
  static constexpr const auto F = false;
  enum e {
    NIL,
    HAND_IDLE,
    HAND_HOLSTER,
    PUNCH_GO,
    PUNCH_BACK,
    HAND_UP,
    MAX,
  };
  struct t {
    e id;
    e next;
    images::e spr;
    bool can_attack;
    bool (*tick)(upc *, float, float);
  };
  constexpr const t all[MAX] = {
    { NIL },
    { HAND_IDLE,    HAND_IDLE,  images::HAND,  T, anims::bob          },
    { HAND_HOLSTER, PUNCH_GO,   images::HAND,  F, anims::hand_holster },
    { PUNCH_GO,     PUNCH_BACK, images::PUNCH, F, anims::punch_go     },
    { PUNCH_BACK,   HAND_UP,    images::PUNCH, F, anims::punch_back   },
    { HAND_UP,      HAND_IDLE,  images::HAND,  T, anims::hand_up      },
  };
  static_assert([]{
    for (auto i = 0; i < MAX; i++) if (all[i].id != i) return false;
    return true;
  }());

  static constexpr const auto start_attack = HAND_HOLSTER;
}
namespace hand {
  stts::e stt = stts::HAND_UP;

  enum class states {
    dummy,
    walking,
    to_attack,
    attacking,
    to_walk,
  } state;

  export void attack() {
    // TODO: move to model and reset T
    if (!stts::all[stt].can_attack) return;
    stt = stts::start_attack; // TODO: introduce "weapon" to hold this
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

    upc m_pc {};
    voo::one_quad m_quad;
    v::ppl_with_txt<upc> m_ppl;
    float m_t = 0;

    hai::array<voo::host_buffer_for_image> m_imgs { images::MAX };

  public:
    explicit model(voo::device_and_queue & dq, vee::image_view::type lgm_iv)
      : m_quad { v::g->pd }
      , m_ppl { &dq, lgm_iv, "hand.png", "hand", {
        .depth_test = false,
        .bindings { m_quad.vertex_input_bind() },
        .attributes { m_quad.vertex_attribute(0) },
      }}
    {
      for (auto i = 0; i < images::MAX; i++) {
        m_imgs[i] = voo::load_image_file_as_buffer(images::all[i].filename, v::g->pd);
      }
    }

    void tick(float ms, bool moved) {
      if (stts::all[stt].tick(&m_pc, m_t, moved)) {
        m_t += ms;
      } else {
        m_t = 0;

        auto old_spr = stts::all[stt].spr;
        stt = stts::all[stt].next;

        auto new_spr = stts::all[stt].spr;
        if (new_spr != old_spr) m_ppl.copy_image(m_imgs[new_spr]);
      }

      // TODO: improve easy in/out of attack
      if (state == states::to_attack) {
        m_pc.pos = dotz::mix(m_pc.pos, holster_pos, ms * holster_speed / 1000.0f);
        if (dotz::length(m_pc.pos - holster_pos) < 0.01) {
          state = states::attacking;
          m_pc.pos = attack_start_pos;
          m_pc.size = attack_size;
          m_ppl.copy_image(m_imgs[images::PUNCH]);
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
          m_ppl.copy_image(m_imgs[images::HAND]);
        }
        return;
      }
    }

    void run(vee::command_buffer cb) {
      m_ppl.cmd_bind(cb, m_pc);
      m_quad.run(cb, 0, 1);
    }
  };
}
