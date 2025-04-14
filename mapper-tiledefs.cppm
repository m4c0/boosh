export module mapper:tiledefs;
import :error;
import :textures;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  export struct tiledef {
    jute::heap id;
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

    void copy(tiledef o) {
      auto & c = current();
      if (o.wall)    c.wall    = o.wall;
      if (o.floor)   c.floor   = o.floor;
      if (o.ceiling) c.ceiling = o.ceiling;
      if (o.walk)    c.walk    = o.walk;

      if (o.entity.size()) c.entity = o.entity;
    }

    [[nodiscard]] constexpr auto & operator[](jute::view id) const {
      for (auto & d: m_defs) {
        if (d.id == id) return d;
      }
      throw error("undefined tiledef: "_hs + id);
    }

  public:
    explicit constexpr tiledefs(textures * t) : m_txts { t } {}

    [[nodiscard]] constexpr auto & operator[](char id) const {
      jute::view v { &id, 1 };
      return (*this)[v];
    };

    void add(jute::view arg) { m_defs.push_back(tiledef { arg.trim() }); }
    void parse(jute::view line) {
      auto [cmd, args] = line.split(' ');
      args = args.trim();

           if (cmd == "wall")    current().wall    = (*m_txts)[args] + 1;
      else if (cmd == "floor")   current().floor   = (*m_txts)[args] + 1;
      else if (cmd == "ceiling") current().ceiling = (*m_txts)[args] + 1;
      else if (cmd == "walk")    current().walk    = true;
      else if (cmd == "copy")    copy((*this)[args.trim()]);
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

      if (c.entity == "pushwall"_hs) {
        if (!c.wall || !c.ceiling) err("pushwall must have all of wall, ceiling and floor"_hs);
      } else if (c.entity == "bullet"_hs) {
        if (c.wall) err("bullet cannot be placed on walls"_hs);
        if (!c.floor) err("bullet requires ceiling and floor"_hs);
      } else if (c.entity.size()) {
        err("invalid entity"_hs);
      } else {
        if (!c.wall && !c.ceiling) err("tile should have a wall or both ceiling and floor"_hs);
      }
    }
  };

}
