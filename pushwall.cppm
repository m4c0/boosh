#pragma leco add_resource "pushwall.obj"
export module pushwall;
import collision;
import dotz;
import hai;
import faces;
import mapper;
import model;
import v;

namespace pushwall {
  export constexpr const auto clid = 'push';

  constexpr const auto wall_speed = 1.0f;
  constexpr const auto max = 128;

  struct item {
    dotz::vec2 pos {};
    dotz::vec2 movement {};
  };

  export class model : public ::model::batch {
    hai::varray<item> m_list { max };

    void load(voo::memiter<mdl> & m) override {
      for (auto &p : m_list) m += { .pos = dotz::vec3 { p.pos.x + 0.5f, 0.0f, p.pos.y + 0.5f } };
    }

  public:
    explicit model() : batch { "pushwall.obj", "Tiles101_1K-JPG_Color.jpg" } {}

    void push(unsigned id) {
      auto from = dotz::floor(v::g->camera.cam.xz());
      auto & i = m_list[id];
      float x = from.x < i.pos.x ? 1 : from.x > i.pos.x ? -1 : 0;
      float y = from.y < i.pos.y ? 1 : from.y > i.pos.y ? -1 : 0;
      i.movement = { x, y };
    }

    void load_map(const mapper::tilemap * map) {
      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity != mapper::entities::PUSHWALL) return;

        dotz::vec2 aa { x, y };
        auto bb = aa + 1;
        collision::entities().add_aabb(aa, bb, clid, m_list.size());
        collision::bodies().add_aabb(aa, bb, clid, m_list.size());
        m_list.push_back(item { aa });
      });
    }

    void tick(float ms) {
      auto m = memiter();
      for (auto i = 0; i < m_list.size(); i++) {
        auto & it = m_list[i];
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
