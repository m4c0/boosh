export module collision;
import dotz;
import hai;

namespace collision {
  struct overflow {};

  class layer {
    hai::varray<dotz::vec4> m_data { 1024 };

    auto add(dotz::vec4 p) {
      if (m_data.size() == m_data.capacity()) throw overflow {};
      m_data.push_back(p);
      return m_data.size();
    }

  public:
    [[nodiscard]] auto add_aabb(dotz::vec2 aa, dotz::vec2 bb) {
      return add(dotz::vec4 { aa, bb });
    }
    [[nodiscard]] auto add_circle(dotz::vec2 c, float r) {
      return add(dotz::vec4 { c, r, 0 });
    }

    [[nodiscard]] unsigned closest(dotz::vec2 p) {
      unsigned res {};
      float d = 1e20;
      for (auto i = 0; i < m_data.size(); i++) {
        auto c = m_data[i];
        float cd = 1e20;
        if (c.w == 0) {
          cd = dotz::length(p - c.xy()) - c.z;
        } else {
          auto aa = c.xy();
          auto bb = c.zw();
          dotz::vec2 cp {
            p.x > bb.x ? p.x < aa.x ? aa.x : p.x : bb.x,
            p.y > bb.y ? p.y < aa.y ? aa.y : p.y : bb.y,
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
}
