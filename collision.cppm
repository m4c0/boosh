export module collision;
import dotz;
import hai;

namespace collision {
  struct overflow {};

  struct item {
    dotz::vec4 fn;
    unsigned type;
    unsigned id;
  };

  class layer {
    hai::varray<item> m_data { 1024 };

    void add(dotz::vec4 p, unsigned type, unsigned id) {
      if (m_data.size() == m_data.capacity()) throw overflow {};
      m_data.push_back(item { p, type, id });
    }

  public:
    void add_aabb(dotz::vec2 aa, dotz::vec2 bb, unsigned type, unsigned id) {
      add(dotz::vec4 { aa, bb }, type, id);
    }
    void add_circle(dotz::vec2 c, float r, unsigned type, unsigned id) {
      add(dotz::vec4 { c, r, 0 }, type, id);
    }

    [[nodiscard]] item closest(dotz::vec2 p, float max_r) {
      item res {};
      float d = max_r;
      for (auto & i : m_data) {
        auto c = i.fn;
        float cd = 1e20;
        if (c.w == 0) {
          cd = dotz::length(p - c.xy()) - c.z;
        } else {
          auto aa = c.xy();
          auto bb = c.zw();
          dotz::vec2 cp {
            p.x > bb.x ? bb.x : (p.x < aa.x ? aa.x : p.x),
            p.y > bb.y ? bb.y : (p.y < aa.y ? aa.y : p.y),
          };
          cd = dotz::length(p - cp);
        }
        if (cd >= d) continue;
        d = cd;
        res = i;
      }
      return res;
    }
  };

  export [[nodiscard]] auto & entities() {
    static layer i {};
    return i;
  }
}
