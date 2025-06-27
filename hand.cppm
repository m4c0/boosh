#pragma leco add_resource "hand.png"
#pragma leco add_resource "hand-attack.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import collision;
import mob;
import ppl_with_txt;
import silog;
import textures;
import v;

namespace hand {
  struct upc {
    dotz::vec2 pos {};
    dotz::vec2 size {};
    dotz::vec2 cam {};
    float cam_rot {};
    unsigned txt {};
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
    jute::view filename;
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
  static constexpr const float hand_attack_radius = 1.0f;

  // TODO: improve easy in/out of attack

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
  static bool punch_damage(upc * pc, float t, float spd) {
    auto i = collision::bodies().hitscan(pc->cam, pc->cam_rot, hand_attack_radius);
    switch (i.item.owner) {
      case 0:
        silog::log(silog::info, ">>>>>>>>>>> MISS");
        break;
      case mob::clid:
        silog::log(silog::info, ">>>>>>>>>>> MOB HIT %x", i.item.id);
        break;
      default:
        silog::log(silog::info, ">>>>>>>>>>> HIT %x %x", i.item.owner, i.item.id);
        break;
    }
    return false;
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
    PUNCH_DMG,
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
    { PUNCH_GO,     PUNCH_DMG,  images::PUNCH, F, anims::punch_go     },
    { PUNCH_DMG,    PUNCH_BACK, images::PUNCH, F, anims::punch_damage },
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

  export void attack() {
    // TODO: move to model and reset T
    if (!stts::all[stt].can_attack) return;
    stt = stts::start_attack; // TODO: introduce "weapon" to hold this
  }

  export class model {
    upc m_pc {};
    voo::one_quad m_quad;
    v::grpl m_ppl;
    float m_t = 0;

    hai::array<voo::host_buffer_for_image> m_imgs { images::MAX };

  public:
    explicit model()
      : m_quad { v::g->pd }
      , m_ppl { ppl_with_txt::create<upc>("hand", {
        .depth_test = false,
        .bindings { m_quad.vertex_input_bind() },
        .attributes { m_quad.vertex_attribute(0) },
      }) }
    {
      m_pc.txt = textures::get("hand.png");
    }

    void tick(float ms, bool moved) {
      m_pc.cam = v::g->camera.cam.xz();
      m_pc.cam_rot = dotz::radians(v::g->camera.angle);

      if (stts::all[stt].tick(&m_pc, m_t, moved)) {
        m_t += ms;
      } else {
        m_t = 0;

        auto old_spr = stts::all[stt].spr;
        stt = stts::all[stt].next;

        auto new_spr = stts::all[stt].spr;
        if (new_spr != old_spr) m_pc.txt = textures::get(images::all[new_spr].filename);
      }
    }

    void run(vee::command_buffer cb) {
      ppl_with_txt::cmd_bind(cb, &m_ppl);
      vee::cmd_push_vertex_constants(cb, *m_ppl.layout, &m_pc);
      m_quad.run(cb, 0, 1);
    }
  };
}
