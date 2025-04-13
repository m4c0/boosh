module mapper:tiledefs;
import :error;
import :textures;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  struct tiledef {
    char id;
    unsigned wall;
    unsigned floor;
    unsigned ceiling;
    bool walk;
  };
  class tiledefs {
    static constexpr const auto max = 128;

    hai::varray<tiledef> m_defs { max };
    textures * m_txts;

    [[nodiscard]] constexpr auto & current() { return m_defs[m_defs.size() - 1]; }

    [[nodiscard]] auto arg_of(jute::view arg) {
      arg = arg.trim();
      if (arg.size() != 1) throw error { "invalid id for tiledef: "_hs + arg };
      return arg[0];
    }

    void copy(tiledef o) {
      o.id = current().id;
      current() = o;
    }

  public:
    explicit constexpr tiledefs(textures * t) : m_txts { t } {}

    [[nodiscard]] constexpr auto & operator[](char id) const {
      for (auto & d: m_defs) {
        if (d.id == id) return d;
      }
      throw error("undefined tiledef: "_hs + id);
    }

    void add(jute::view arg) { m_defs.push_back(tiledef { arg_of(arg) }); }
    void parse(jute::view line) {
      auto [cmd, args] = line.split(' ');
           if (cmd == "wall")    current().wall    = (*m_txts)[args.trim()];
      else if (cmd == "floor")   current().floor   = (*m_txts)[args.trim()];
      else if (cmd == "ceiling") current().ceiling = (*m_txts)[args.trim()];
      else if (cmd == "walk")    current().walk    = true;
      else if (cmd == "copy")    copy((*this)[arg_of(args)]);
      else throw error { "unknown command: "_hs + cmd };
    }
  };

}
