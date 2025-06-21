#pragma leco add_resource "pushwall.obj"
export module pushwall;
import collision;
import dotz;
import jute;
import mapper;
import model;
import textures;
import v;

namespace pushwall {
  export constexpr const auto clid = 'push';

  constexpr const auto wall_speed = 1.0f;

  export struct item {
    static constexpr const jute::view entity = "pushwall";
    static constexpr const jute::view model = "pushwall.obj";

    dotz::vec2 pos {};
    dotz::vec2 movement {};
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
  template<> void tick(pushwall::item & it, mdl & m, int id, float ms) {
    if (0 == dotz::length(it.movement)) return;

    auto aa = it.pos + it.movement * pushwall::wall_speed * ms / 1000.0;
    auto bb = it.pos + 1;
    bool hit = false;
    collision::bodies().collides_aabb(aa, bb, [&](auto type, auto id) {
      if (type == pushwall::clid && id == id) return true;
      hit = true;
      return false;
    });
    if (hit) {
      it.movement = {};
      aa = dotz::floor(it.pos);
      bb = aa + 1;
      collision::entities().remove(pushwall::clid, id);
    } else {
      collision::entities().set_aabb(aa, bb, pushwall::clid, id);
    }

    collision::bodies().set_aabb(it.pos, bb, pushwall::clid, id);

    it.pos = aa;
    m.pos = { aa.x + 0.5f, 0.0f, aa.y + 0.5f };
  }
  template<> void use(pushwall::item & i) {
    auto from = dotz::floor(v::g->camera.cam.xz());
    float x = from.x < i.pos.x ? 1 : from.x > i.pos.x ? -1 : 0;
    float y = from.y < i.pos.y ? 1 : from.y > i.pos.y ? -1 : 0;
    i.movement = { x, y };
  }
}
