export module pushwall;
import collision;
import dotz;
import faces;
import hai;

namespace pushwall {
  export constexpr const auto clid = 'push';

  constexpr const auto wall_speed = 1.0f;
  constexpr const auto max = 128;

  struct item {
    dotz::vec2 pos {};
    unsigned iid {};
    dotz::vec2 movement {};
  };
  hai::varray<item> list { max };

  export void add(dotz::vec2 aa, unsigned instance) { 
    auto bb = aa + 1;
    collision::entities().add_aabb(aa, bb, clid, list.size());
    collision::bodies().add_aabb(aa, bb, clid, list.size());
    list.push_back(item { aa, instance });
  }

  export void push(dotz::vec2 from, unsigned id) {
    auto & i = list[id];
    from = dotz::floor(from);
    float x = from.x < i.pos.x ? 1 : from.x > i.pos.x ? -1 : 0;
    float y = from.y < i.pos.y ? 1 : from.y > i.pos.y ? -1 : 0;
    i.movement = { x, y };
  }

  export void tick(faces::wall & walls, float ms) {
    auto w = walls.remap();
    for (auto i = 0; i < list.size(); i++) {
      auto & it = list[i];
      if (0 == dotz::length(it.movement)) continue;
      it.pos = it.pos + it.movement * wall_speed * ms / 1000.0;
      auto bb = it.pos + 1;
      collision::entities().set_aabb(it.pos, bb, clid, i);
      collision::bodies().set_aabb(it.pos, bb, clid, i);

      w[it.iid].pos.x = it.pos.x;
      w[it.iid].pos.z = it.pos.y;
    }
  }
}
