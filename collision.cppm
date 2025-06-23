export module collision;
import dotz;
import hai;
import silog;

namespace collision {
  struct overflow {};

  enum class type { none, circle, aabb };

  struct item {
    dotz::vec4 fn;
    type type;
    unsigned owner;
    unsigned id;
  };
  struct hit {
    item item;
    float dist;
  };

  class layer {
    hai::varray<item> m_data { 1024 };

    void add(item it) {
      if (m_data.size() == m_data.capacity()) throw overflow {};
      m_data.push_back(it);
    }
    void set(item it) {
      for (auto i = 0; i < m_data.size(); i++) {
        if (m_data[i].owner != it.owner) continue;
        if (m_data[i].id != it.id) continue;
        m_data[i] = it;
        return;
      }
    }

  public:
    void add_aabb(dotz::vec2 aa, dotz::vec2 bb, unsigned owner, unsigned id) {
      add(item { { aa, bb }, type::aabb, owner, id });
    }
    void add_circle(dotz::vec2 c, float r, unsigned owner, unsigned id) {
      add(item { { c, r, 0 }, type::circle, owner, id });
    }

    void set_aabb(dotz::vec2 aa, dotz::vec2 bb, unsigned owner, unsigned id) {
      set(item { { aa, bb }, type::aabb, owner, id });
    }
    void set_circle(dotz::vec2 c, float r, unsigned owner, unsigned id) {
      set(item { { c, r, 0 }, type::circle, owner, id });
    }

    void remove(unsigned owner, unsigned id) {
      set(item { {}, type::none, owner, id });
    }

    [[nodiscard]] hit hitscan(dotz::vec2 p, float rad, float max_dist) {
      dotz::vec2 l { dotz::sinf(rad), dotz::cosf(rad) };

      hit res {
        .dist = max_dist,
      };
      for (auto & i : m_data) {
        switch (i.type) {
          case type::none: break;
          case type::circle: {
            auto t = dotz::dot(l, i.fn.xy() - p);
            if (t < 0 || t > res.dist) continue;
            auto pp = p + l * t;
            if (dotz::length(pp - i.fn.xy()) > i.fn.z) continue;

            res.dist = t;
            res.item = i;
            break;
          }
          case type::aabb: {
            auto tlow  = (i.fn.xy() - p) / l;
            auto thigh = (i.fn.zw() - p) / l;
            auto tclose = dotz::min(tlow, thigh);
            auto tfar   = dotz::max(tlow, thigh);
            auto tc = dotz::max(tclose.x, tclose.y);
            auto tf = dotz::min(tfar.x, tfar.y);
            if (tc < 0 || tc > tf || tc > res.dist) continue;

            res.dist = tc;
            res.item = i;
            break;
          }
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
        switch (i.type) {
          case type::none: break;
          case type::circle: {
            cd = dotz::length(p - c.xy()) - c.z;
            break;
          }
          case type::aabb: {
            auto aa = c.xy();
            auto bb = c.zw();
            dotz::vec2 cp {
              p.x > bb.x ? bb.x : (p.x < aa.x ? aa.x : p.x),
                p.y > bb.y ? bb.y : (p.y < aa.y ? aa.y : p.y),
            };
            cd = dotz::length(p - cp);
            break;
          }
        }
        if (cd >= d) continue;
        d = cd;
        res = i;
      }
      return res;
    }

    void collides_aabb(dotz::vec2 aa, dotz::vec2 bb, auto fn) {
      for (auto & i : m_data) {
        switch (i.type) {
          case type::none: break;
          case type::circle: {
            auto p = i.fn.xy() - (aa + bb) / 2.0f;
            auto b = dotz::abs(bb - aa) / 2.0f;
            auto d = dotz::abs(p) - b;
            float sd = dotz::length(dotz::max(d, dotz::vec2(0)))
              + dotz::min(dotz::max(d.x, d.y), 0.0);;
            if (sd - i.fn.z > 0) continue;
            if (!fn(i.owner, i.id)) return;
            break;
          }
          case type::aabb: {
            auto iaa = i.fn.xy();
            auto ibb = i.fn.zw();
            if (iaa.x >= bb.x) continue;
            if (ibb.x <= aa.x) continue;
            if (iaa.y >= bb.y) continue;
            if (ibb.y <= aa.y) continue;
            if (!fn(i.owner, i.id)) return;
            break;
          }
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
