#pragma leco add_resource "bullet.obj"
#pragma leco add_resource "bullet.uv.png"
export module bullet;
import collision;
import dotz;
import jute;
import model;
import mapper;
import textures;

namespace bullet {
  export constexpr const auto clid = 'bllt';

  static constexpr const auto radius = 0.2;

  export struct item : dotz::vec3 {
    static constexpr const jute::view entity = "bullet";
    static constexpr const jute::view model = "bullet.obj";

    using vec3::vec3;
  };
}
namespace model {
  template<> mdl convert(bullet::item p) {
    auto txt = textures::get("bullet.uv.png");
    return { .pos = p, .txt = txt };
  }

  template<> bullet::item create(int x, int y, int id, mapper::tiledef map) {
    bullet::item p { x + 0.5f, 0.0f, y + 0.5f };
    collision::entities().add_circle({ p.x, p.z }, bullet::radius, bullet::clid, id);
    return p;
  }

  template<> void remove(int id, bullet::item _) {
    collision::entities().remove(bullet::clid, id);
  }
}
