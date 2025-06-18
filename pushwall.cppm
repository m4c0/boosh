#pragma leco add_resource "pushwall.obj"
export module pushwall;
import collision;
import dotz;
import hai;
import mapper;
import model;
import textures;
import v;

namespace pushwall {
  export constexpr const auto clid = 'push';

  constexpr const auto wall_speed = 1.0f;

  struct item {
    static constexpr const auto entity = mapper::entities::PUSHWALL;

    dotz::vec2 pos {};
    dotz::vec2 movement {};
  };

  export class model : public ::model::list<item> {
  public:
    explicit model() : list { "pushwall.obj" } {}

    void push(unsigned id) {
      auto from = dotz::floor(v::g->camera.cam.xz());
      auto & i = data()[id];
      float x = from.x < i.pos.x ? 1 : from.x > i.pos.x ? -1 : 0;
      float y = from.y < i.pos.y ? 1 : from.y > i.pos.y ? -1 : 0;
      i.movement = { x, y };
    }

    void tick(float ms) {
      auto m = memiter();
      for (auto i = 0; i < data().size(); i++) {
        auto & it = data()[i];
        if (0 == dotz::length(it.movement)) continue;

        auto aa = it.pos + it.movement * wall_speed * ms / 1000.0;
        auto bb = it.pos + 1;
        bool hit = false;
        collision::bodies().collides_aabb(aa, bb, [&](auto type, auto id) {
          if (type == clid && id == i) return true;
          hit = true;
          return false;
        });
        if (hit) {
          it.movement = {};
          aa = dotz::floor(it.pos);
          bb = aa + 1;
          collision::entities().remove(clid, i);
        } else {
          collision::entities().set_aabb(aa, bb, clid, i);
        }

        collision::bodies().set_aabb(it.pos, bb, clid, i);

        it.pos = aa;
        m[i].pos = { aa.x + 0.5f, 0.0f, aa.y + 0.5f };
      }
    }
  };
}
namespace model {
  template<> mdl convert(pushwall::item p) {
    auto txt = textures::get("Tiles101_1K-JPG_Color.jpg");
    return {
      .pos = dotz::vec3 { p.pos.x + 0.5f, 0.0f, p.pos.y + 0.5f },
      .txt = txt,
    };
  }
  template<> pushwall::item create(int x, int y, int id, mapper::tiledef d) {
    dotz::vec2 aa { x, y };
    auto bb = aa + 1;
    collision::entities().add_aabb(aa, bb, pushwall::clid, id);
    collision::bodies().add_aabb(aa, bb, pushwall::clid, id);
    return pushwall::item { aa };
  }
}
