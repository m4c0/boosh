#pragma leco add_resource "bullet.obj"
#pragma leco add_resource "bullet.uv.png"
export module bullet;
import collision;
import dotz;
import hai;
import model;
import mapper;
import textures;
import voo;

namespace bullet {
  struct item : dotz::vec3 {
    using vec3::vec3;
  };
}
namespace model {
  template<> mdl convert(bullet::item p) {
    auto txt = textures::get("bullet.uv.png");
    return { .pos = p, .txt = txt };
  }
}
namespace bullet {
  export constexpr const auto clid = 'bllt';

  static constexpr const auto radius = 0.2;

  export class model : public ::model::list<item> {
  public:
    explicit model() : list { "bullet.obj" } {}

    void remove(int id) {
      collision::entities().remove(clid, id);
      data()[id] = {};
    }

    void load_map(const mapper::tilemap * map) {
      for (auto id = 0; id < data().size(); id++) remove(id);
      data().truncate(0);

      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity != mapper::entities::BULLET) return;

        item p { x + 0.5f, 0.0f, y + 0.5f };
        collision::entities().add_circle({ p.x, p.z }, radius, clid, data().size());
        data().push_back(p);
      });
    }
  };
}
