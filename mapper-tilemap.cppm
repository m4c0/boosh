export module mapper:tilemap;
import :error;
export import :tiledefs;
import jute;
import hai;

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

    void for_each(auto && fn) const {
      for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
          fn(x, y, (*this)(x, y));
        }
      }
    }
  };
}
