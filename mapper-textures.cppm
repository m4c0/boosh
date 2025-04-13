module mapper:textures;
import :error;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  struct texture {
    jute::heap id;
    jute::heap name;
  };
  class textures {
    static constexpr const auto max = 128;
    hai::varray<texture> m_list { max };

  public:
    [[nodiscard]] constexpr auto operator[](jute::view id) const {
      for (auto i = 0; i < m_list.size(); i++) {
        if (id == m_list[i].id) return i;
      }
      throw error { "texture not defined: "_hs + id };
    }
    void add(jute::view arg) {
      auto [id, name] = arg.split(' ');

      id = id.trim();
      if (id == "") throw error { "missing texture id"_hs };

      name = name.trim();
      if (name == "") throw error { "missing texture name"_hs };

      for (auto &[i, n] : m_list) {
        if (i == id) throw error { "duplicate texture id: "_hs + id };
        if (n == name) throw error { "duplicate texture name: "_hs + name };
      }
      m_list.push_back(texture { id, name });
    }

    [[nodiscard]] constexpr auto take_list() const {
      hai::array<jute::heap> list { m_list.size() };
      for (auto i = 0; i < m_list.size(); i++) list[i] = m_list[i].name;
      return list;
    }
  };
}
