export module collision;
import dotz;
import hai;

namespace collision {
  static constexpr const auto width = 256;
  static constexpr const auto height = 256;

  struct err {};

  struct entity {
    unsigned id;
    dotz::vec2 pos;
  };
  struct node {
    hai::varray<entity> data {};
  };
  class map {
    hai::array<node> m_data { width * height };

    auto & nodeof(const entity & e) {
      int x = e.pos.x;
      int y = e.pos.y;
      if (x < 0 || y < 0 || x >= width || y >= height) throw err {};
      return m_data[y * width + x];
    }

  public:
    void add(const entity & e) {
      nodeof(e).data.push_back_doubling(e);
    }

    void remove(const entity & e) {
      auto & d = nodeof(e).data;
      for (auto i = 0; i < d.size(); i++) {
        if (d[i].id != e.id) continue;
        d[i] = d.pop_back();
        return;
      }
    }
  };
}
