export module mapper:tiledefs;
import :error;
import :textures;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  export struct tiledef {
    char id;
    unsigned wall;
    unsigned floor;
    unsigned ceiling;
    jute::heap entity {};
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
      args = args.trim();

           if (cmd == "wall")    current().wall    = (*m_txts)[args] + 1;
      else if (cmd == "floor")   current().floor   = (*m_txts)[args] + 1;
      else if (cmd == "ceiling") current().ceiling = (*m_txts)[args] + 1;
      else if (cmd == "walk")    current().walk    = true;
      else if (cmd == "copy")    copy((*this)[arg_of(args)]);
      else if (cmd == "entity")  current().entity  = args;
      else throw error { "unknown command: "_hs + cmd };
    }

    void validate_last() {
      auto & c = current();
      const auto err = [&](jute::heap msg) {
        throw error { msg + " for tiledef [" + c.id + "]" };
      };

      if (c.ceiling && !c.floor) err("floor must be defined when ceiling is"_hs);
      if (!c.ceiling && c.floor) err("ceiling must be defined when floor is"_hs);

      auto entity = c.entity != ""_hs;
      if (c.wall && entity) err("entity cannot be placed on walls"_hs);
      if (entity && !c.floor) err("entity requires ceiling and floor"_hs);

      if (!c.wall && !c.ceiling) err("tile should have a wall or both ceiling and floor"_hs);
    }
  };

}
