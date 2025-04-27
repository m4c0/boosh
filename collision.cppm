export module collision;
import dotz;
import hai;

namespace collision {
  struct overflow {};

  struct item {
    dotz::vec4 fn;
    unsigned owner;
    unsigned id;
  };

  class layer {
    hai::varray<item> m_data { 1024 };

    void add(dotz::vec4 p, unsigned owner, unsigned id) {
      if (m_data.size() == m_data.capacity()) throw overflow {};
      m_data.push_back(item { p, owner, id });
    }
    void set(dotz::vec4 p, unsigned owner, unsigned id) {
      for (auto i = 0; i < m_data.size(); i++) {
        if (m_data[i].owner != owner) continue;
        if (m_data[i].id != id) continue;
        m_data[i].fn = p;
        return;
      }
    }

  public:
    void add_aabb(dotz::vec2 aa, dotz::vec2 bb, unsigned owner, unsigned id) {
      add(dotz::vec4 { aa, bb }, owner, id);
    }
    void add_circle(dotz::vec2 c, float r, unsigned owner, unsigned id) {
      add(dotz::vec4 { c, r, 0 }, owner, id);
    }

    void set_aabb(dotz::vec2 aa, dotz::vec2 bb, unsigned owner, unsigned id) {
      set(dotz::vec4 { aa, bb }, owner, id);
    }
    void set_circle(dotz::vec2 c, float r, unsigned owner, unsigned id) {
      set(dotz::vec4 { c, r, 0 }, owner, id);
    }

    void remove(unsigned owner, unsigned id) {
      set(dotz::vec4 {}, owner, id);
    }

    [[nodiscard]] item hitscan(dotz::vec2 p, float rad, float max_dist) {
      dotz::vec2 l { dotz::sinf(rad), dotz::cosf(rad) };

      item res {};
      float min_dist = max_dist;
      for (auto & i : m_data) {
        if (i.fn.w == 0) {
          auto t = dotz::dot(l, i.fn.xy() - p);
          if (t < 0 || t > min_dist) continue;
          auto pp = p + l * t;
          if (dotz::length(pp - i.fn.xy()) > i.fn.z) continue;

          min_dist = t;
          res = i;
        } else {
          auto tlow  = (i.fn.xy() - p) / l;
          auto thigh = (i.fn.zw() - p) / l;
          auto tclose = dotz::min(tlow, thigh);
          auto tfar   = dotz::max(tlow, thigh);
          auto tc = dotz::max(tclose.x, tclose.y);
          auto tf = dotz::min(tfar.x, tfar.y);
          if (tc < 0 || tc > tf || tc > min_dist) continue;

          min_dist = tc;
          res = i;
        }
      }
      return res;
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

    void collides_aabb(dotz::vec2 aa, dotz::vec2 bb, auto fn) {
      for (auto & i : m_data) {
        if (i.fn.w == 0) {
          // TODO: sdf between mid(aa, bb) and i.fn.xy
          throw 0;
        } else {
          auto iaa = i.fn.xy();
          auto ibb = i.fn.zw();
          if (iaa.x >= bb.x) continue;
          if (ibb.x <= aa.x) continue;
          if (iaa.y >= bb.y) continue;
          if (ibb.y <= aa.y) continue;
          if (!fn(i.owner, i.id)) return;
        }
      }
    }
  };

  export [[nodiscard]] auto & bodies() {
    static layer i {};
    return i;
  }
  export [[nodiscard]] auto & entities() {
    static layer i {};
    return i;
  }
}
