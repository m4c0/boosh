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
  export constexpr const auto clid = 'bllt';

  static constexpr const auto radius = 0.2;

  struct item : dotz::vec3 {
    static constexpr const auto entity = mapper::entities::BULLET;

    using vec3::vec3;
  };

  export class model : public ::model::list<item> {
  public:
    explicit model() : list { "bullet.obj" } {}
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
