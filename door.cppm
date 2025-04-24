#pragma leco add_resource "door.obj"
#pragma leco add_resource "door.uv.png"
export module door;
import dotz;
import hai;
import model;
import voo;

namespace door {
  export constexpr const auto clid = 'door';

  hai::varray<dotz::vec3> list { 128 };

  export void add(dotz::vec3 p) {
    list.push_back(p);
  }

  export class model : public ::model::batch {
    void load(voo::memiter<mdl> & m) override {
      for (auto p : list) m += { .pos = p };
    }

  public:
    explicit model(voo::device_and_queue & dq)
      : batch { dq, "door.obj", "door.uv.png" }
    {}
  };
}
