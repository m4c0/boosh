#pragma leco add_resource "bullet.obj"
#pragma leco add_resource "bullet.uv.png"
export module bullet;
import collision;
import dotz;
import hai;
import model;
import voo;

namespace bullet {
  export constexpr const auto clid = 'bllt';

  hai::varray<dotz::vec3> list { 128 };

  export void add(dotz::vec3 p) { 
    collision::entities().add_circle({ p.x, p.z }, 0.1, clid, list.size());
    list.push_back(p);
  }
  export void remove(unsigned id) {
    collision::entities().remove(clid, id);
    list[id] = {};
  }

  export class model : public ::model::batch {
    void load(voo::memiter<mdl> & m) override {
      for (auto p : list) m += { .pos = p };
    }

  public:
    explicit model(voo::device_and_queue & dq, auto * k)
      : batch { dq, k, "bullet.obj", "bullet.uv.png" }
    {}
  };
}
