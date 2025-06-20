#pragma leco add_resource "mob.obj"
export module mob;
import collision;
import dotz;
import jute;
import mapper;
import model;
import textures;

namespace mob {
  export constexpr const auto clid = 'mobs';

  static constexpr const auto radius = 0.3;

  export struct item {
    static constexpr const auto entity = mapper::entities::MOB;
    static constexpr const jute::view model = "mob.obj";

    dotz::vec3 pos;
  };
}

namespace model {
  template<> mdl convert(mob::item p) {
    // TODO: own texture
    auto txt = textures::get("door.uv.png");
    return { .pos = p.pos, .txt = txt };
  }

  template<> mob::item create(int x, int y, int id, mapper::tiledef d) {
    dotz::vec3 p { x + 0.5f, 0.0f, y + 0.5f };
    collision::bodies().add_circle({ p.x, p.z }, mob::radius, mob::clid, id);
    collision::entities().add_circle({ p.x, p.z }, mob::radius, mob::clid, id);
    return { p };
  }
}
