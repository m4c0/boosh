#pragma leco add_resource "mob.obj"
export module mob;
import collision;
import dotz;
import hai;
import mapper;
import model;
import textures;
import voo;

namespace model {
  template<> mdl convert(dotz::vec3 p) {
    // TODO: own texture
    auto txt = textures::get("door.uv.png");
    return { .pos = p, .txt = txt };
  }
}
namespace mob {
  export constexpr const auto clid = 'mobs';

  static constexpr const auto radius = 0.3;

  export class model : public ::model::list<dotz::vec3> {
  public:
    explicit model() : list { "mob.obj" } {}

    void load_map(const mapper::tilemap * map) {
      map->for_each([&](auto x, auto y, auto & d) {
          return;
        if (d.entity != mapper::entities::BULLET) return;

        dotz::vec3 p { x + 0.5f, 0.0f, y + 0.5f };
        collision::bodies().add_circle({ p.x, p.z }, radius, clid, data().size());
        collision::entities().add_circle({ p.x, p.z }, radius, clid, data().size());
        data().push_back(p);
      });
    }
  };
}
