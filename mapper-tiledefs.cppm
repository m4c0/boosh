export module mapper:tiledefs;
import :error;
import :textures;
import entdefs;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  export struct tiledef {
    jute::heap id;
    jute::heap floor;
    jute::heap ceiling;
    unsigned rotate;
    const entdefs::t * entity {};
    unsigned light;
    bool walk;
  };

  static auto parse_entities(jute::view value) {
    auto e = entdefs::for_name(value);
    if (!e) throw error { "unknown entity: "_hs + value };
    return e;
  }

  class tiledefs {
    static constexpr const auto max = 128;

    hai::varray<tiledef> m_defs { max };

    [[nodiscard]] constexpr auto & current() { return m_defs[m_defs.size() - 1]; }

    void copy(tiledef o) {
      auto & c = current();
      if (o.floor.size())   c.floor   = o.floor;
      if (o.ceiling.size()) c.ceiling = o.ceiling;
      if (o.rotate)  c.rotate  = o.rotate;
      if (o.walk)    c.walk    = o.walk;
      if (o.light)   c.light   = o.light;

      if (!!o.entity) c.entity = o.entity;
    }

    [[nodiscard]] constexpr auto & operator[](jute::view id) const {
      for (auto & d: m_defs) {
        if (d.id == id) return d;
      }
      throw error("undefined tiledef: "_hs + id);
    }

  public:
    [[nodiscard]] constexpr auto & operator[](char id) const {
      jute::view v { &id, 1 };
      return (*this)[v];
    };

    void add(jute::view arg) { m_defs.push_back(tiledef { arg.trim() }); }
    void parse(jute::view line) {
      auto [cmd, args] = line.split(' ');
      args = args.trim();

           if (cmd == "floor")   current().floor   = args;
      else if (cmd == "ceiling") current().ceiling = args;
      else if (cmd == "rotate")  current().rotate  = jute::to_f(args);
      else if (cmd == "walk")    current().walk    = true;
      else if (cmd == "light")   current().light   = jute::to_f(args);
      else if (cmd == "copy")    copy((*this)[args.trim()]);
      else if (cmd == "entity")  current().entity  = parse_entities(args);
      else throw error { "unknown command: "_hs + cmd };
    }

    void validate_last() {
      auto & c = current();
      const auto err = [&](jute::heap msg) {
        throw error { msg + " for tiledef [" + c.id + "]" };
      };

      if (c.ceiling.size() && !c.floor.size()) err("floor must be defined when ceiling is"_hs);
      if (!c.ceiling.size() && c.floor.size()) err("ceiling must be defined when floor is"_hs);

      if (c.light < 0 || c.light > 255) err("light should be between 0 and 255"_hs);

      if (!c.entity) {
        if (c.rotate) err("empty tiles cannot be rotated"_hs);
        if (!c.ceiling.size()) err("empty tiles must have floor or ceiling"_hs);
        return;
      }

      auto e = c.entity;

      switch (e->rotates) {
        using namespace entdefs;
        case rotation::NONE:   if (c.rotate) err(e->name + " cannot be rotated");
        case rotation::FIXED:  if (c.rotate != 0 && c.rotate != 90) err(e->name + " can only be rotated 0 or 90 degrees");
        case rotation::FREELY: break;
      }

      if (c.ceiling.size() && !e->grounded) err(e->name + " cannot have floor or ceiling");
      if (!c.ceiling.size() && e->grounded) err(e->name + " must have floor or ceiling");
    }
  };

}
