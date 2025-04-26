#pragma leco add_resource "door.obj"
#pragma leco add_resource "door.uv.png"
export module door;
import collision;
import dotz;
import hai;
import model;
import voo;

namespace door {
  export constexpr const auto clid = 'door';

  constexpr const auto door_speed = 1.0f;
  constexpr const auto stop_y = 0.9f;
  constexpr const auto max = 128;

  struct item {
    dotz::vec3 pos;
    float rot;
    float movement;
  };
  hai::varray<item> list { max };

  export void add(dotz::vec3 p, float r) {
    dotz::vec2 aa = dotz::floor(p.xz());
    dotz::vec2 bb = aa + 1;
    if (r == 0) {
      aa.x += 0.4;
      bb.x = aa.x + 0.2;
    } else {
      aa.y += 0.4;
      bb.y = aa.y + 0.2;
    }
    collision::bodies().add_aabb(aa, bb, clid, list.size());
    collision::entities().add_aabb(aa, bb, clid, list.size());
    list.push_back(item { p, r, 0 });
  }

  export void open(unsigned id) {
    list[id].movement = 1;
  }

  export class model : public ::model::batch {
    void load(voo::memiter<mdl> & m) override {
      for (auto p : list) m += { .pos = p.pos, .rot = p.rot };
    }

  public:
    explicit model(voo::device_and_queue & dq, auto * k)
      : batch { dq, k, "door.obj", "door.uv.png" }
    {}

    void tick(float ms) {
      auto m = memiter();
      for (auto i = 0; i < list.size(); i++) {
        auto & it = list[i];
        if (it.movement == 0) continue;

        float y = it.pos.y - door_speed * ms / 1000.0;
        if (y < -stop_y) {
          y = -stop_y;
          it.movement = 0;
          collision::bodies().remove(clid, i);
          collision::entities().remove(clid, i);
        }
        m[i].pos.y = it.pos.y = y;
      }
    }
  };
}
