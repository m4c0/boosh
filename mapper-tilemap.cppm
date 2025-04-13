export module mapper:tilemap;
export import :tiledefs;

using namespace jute::literals;

namespace mapper {
  export class tilemap {
    static constexpr const auto width = 256;
    static constexpr const auto height = 256;

    hai::array<tiledef> m_data { width * height };
  public:
    [[nodiscard]] constexpr const auto & operator()(unsigned x, unsigned y) const {
      if (x >= width || y >= height) throw error { "out of map bounds"_hs };
      return m_data[y * width + x];
    }
    [[nodiscard]] constexpr auto & operator()(unsigned x, unsigned y) {
      if (x >= width || y >= height) throw error { "out of map bounds"_hs };
      return m_data[y * width + x];
    }

    void find_entities(jute::view name, auto && fn) {
      for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
          const auto & d = (*this)(x, y);
          if (d.entity == name) fn(x, y, d);
        }
      }
    }
  };
}
