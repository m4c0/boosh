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

  hai::varray<dotz::vec4> list { 128 };

  export void add(dotz::vec3 p, float r) {
    list.push_back(dotz::vec4 { p, r });

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
  }

  export class model : public ::model::batch {
    void load(voo::memiter<mdl> & m) override {
      for (auto p : list) m += { .pos = p.xyz(), .rot = p.w };
    }

  public:
    explicit model(voo::device_and_queue & dq, auto * k)
      : batch { dq, k, "door.obj", "door.uv.png" }
    {}
  };
}
