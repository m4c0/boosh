#pragma leco add_resource "bullet.obj"
#pragma leco add_resource "bullet.uv.png"
export module bullet;
import collision;
import dotz;
import hai;
import model;
import mapper;
import voo;

namespace bullet {
  export constexpr const auto clid = 'bllt';

  static constexpr const auto radius = 0.2;

  export class model : public ::model::batch {
    hai::varray<dotz::vec3> m_list { 128 };

    void load(voo::memiter<mdl> & m) override {
      for (auto p : m_list) m += { .pos = p };
    }

  public:
    explicit model() : batch { "bullet.obj", "bullet.uv.png" } {}

    void remove(int id) {
      collision::entities().remove(clid, id);
      m_list[id] = 0;
    }

    void load_map(const mapper::tilemap * map) {
      for (auto id = 0; id < m_list.size(); id++) remove(id);
      m_list.truncate(0);

      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity != mapper::entities::BULLET) return;

        dotz::vec3 p { x + 0.5f, 0.0f, y + 0.5f };
        collision::entities().add_circle({ p.x, p.z }, radius, clid, m_list.size());
        m_list.push_back(p);
      });
    }
  };
}
