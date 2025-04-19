export module pushwall;
import collision;
import dotz;
import hai;

namespace pushwall {
  export constexpr const auto clid = 'push';

  constexpr const auto max = 128;

  hai::varray<dotz::vec2> list { max };

  export void add(dotz::vec2 aa) { 
    auto bb = aa + 1;
    collision::entities().add_aabb(aa, bb, clid, list.size());
    list.push_back(aa);
  }
  export void remove(unsigned id) {
    collision::entities().remove(clid, id);
    list[id] = {};
  }
}
