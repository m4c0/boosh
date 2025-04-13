export module mapper:tilemap;
import :tiledefs;

using namespace jute::literals;

namespace mapper {
  export class tilemap {
    static constexpr const auto width = 256;
    static constexpr const auto height = 256;

    tiledef m_data[height][width];
  public:
    [[nodiscard]] constexpr const auto & operator()(unsigned x, unsigned y) const {
      if (x >= width || y >= height) throw error { "out of map bounds"_hs };
      return m_data[y][x];
    }
    [[nodiscard]] constexpr auto & operator()(unsigned x, unsigned y) {
      if (x >= width || y >= height) throw error { "out of map bounds"_hs };
      return m_data[y][x];
    }
  };
}
