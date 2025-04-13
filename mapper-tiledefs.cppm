module mapper:tiledefs;
import :error;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  struct tiledef {
    char id;
  };
  class tiledefs {
    static constexpr const auto max = 128;

    hai::varray<tiledef> m_defs { max };
  public:
    [[nodiscard]] constexpr auto & current() { return m_defs[m_defs.size() - 1]; }

    void add(jute::view arg) {
      arg = arg.trim();
      if (arg.size() != 1) throw error { "invalid id for tiledef: "_hs + arg };
      m_defs.push_back(tiledef { arg[0] });
    }
  };

}
