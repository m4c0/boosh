#pragma leco add_resource "mob.obj"
export module mob;
import collision;
import dotz;
import hai;
import mapper;
import model;
import textures;
import voo;

namespace mob {
  export constexpr const auto clid = 'mobs';

  static constexpr const auto radius = 0.3;

  export class model : public ::model::batch {
    hai::varray<dotz::vec3> m_list { 128 };

    void load(voo::memiter<mdl> & m) override {
      // TODO: own texture
      auto txt = textures::get("door.uv.png");
      for (auto p : m_list) m += { .pos = p, .txt = txt };
    }

  public:
    explicit model() : batch { "mob.obj" } {}

    void load_map(const mapper::tilemap * map) {
      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity != mapper::entities::BULLET) return;

        dotz::vec3 p { x + 0.5f, 0.0f, y + 0.5f };
        collision::bodies().add_circle({ p.x, p.z }, radius, clid, m_list.size());
        collision::entities().add_circle({ p.x, p.z }, radius, clid, m_list.size());
        m_list.push_back(p);
      });
    }
  };
}
