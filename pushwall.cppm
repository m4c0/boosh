export module pushwall;
import collision;
import dotz;
import hai;

namespace pushwall {
  export constexpr const auto clid = 'push';

  constexpr const auto max = 128;

  struct item {
    dotz::vec2 pos;
    unsigned iid;
  };
  hai::varray<item> list { max };

  export void add(dotz::vec2 aa, unsigned instance) { 
    auto bb = aa + 1;
    collision::entities().add_aabb(aa, bb, clid, list.size());
    collision::bodies().add_aabb(aa, bb, clid, list.size());
    list.push_back(item { aa, instance });
  }
  export void remove(unsigned id) {
    collision::entities().remove(clid, id);
    list[id] = {};
  }
}
