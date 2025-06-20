export module mapper:tiledefs;
import :error;
import :textures;
import hai;
import jute;

using namespace jute::literals;

namespace mapper {
  export enum class entities {
    NONE,
    BULLET,
    DOOR,
    MOB,
    PLAYER,
    PUSHWALL,
    WALL,
  };
  export struct tiledef {
    jute::heap id;
    jute::heap wall;
    jute::heap floor;
    jute::heap ceiling;
    unsigned rotate;
    entities entity = entities::NONE;
    unsigned light;
    bool walk;
  };

  export constexpr bool operator!(entities e) { return e == entities::NONE; }
  static constexpr entities parse_entities(jute::view value) {
    if (value == "bullet")   return entities::BULLET;
    if (value == "door")     return entities::DOOR;
    if (value == "mob")      return entities::MOB;
    if (value == "player")   return entities::PLAYER;
    if (value == "pushwall") return entities::PUSHWALL;
    if (value == "wall")     return entities::WALL;

    throw error { "unknown entity: "_hs + value };
  }

  class tiledefs {
    static constexpr const auto max = 128;

    hai::varray<tiledef> m_defs { max };

    [[nodiscard]] constexpr auto & current() { return m_defs[m_defs.size() - 1]; }

    void copy(tiledef o) {
      auto & c = current();
      if (o.wall.size())    c.wall    = o.wall;
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

           if (cmd == "wall")    current().wall    = args;
      else if (cmd == "floor")   current().floor   = args;
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

      switch (c.entity) {
        case entities::PUSHWALL:
          if (c.rotate) err("pushwalls can't be rotated yet"_hs);
          if (c.wall.size()) err("pushwall cannot have a wall attribute"_hs);
          if (!c.ceiling.size()) err("pushwall must have ceiling and floor"_hs);
          break;
        case entities::WALL:
          if (c.rotate) err("walls can't be rotated yet"_hs);
          if (!c.wall.size()) err("wall must have a wall attribute"_hs);
          if (c.floor.size()) err("wall cannot have floor or ceiling"_hs);
          break;
        case entities::BULLET:
          if (c.rotate) err("bullets can't be rotated yet"_hs);
          if (c.wall.size()) err("bullet cannot be placed on walls"_hs);
          if (!c.floor.size()) err("bullet requires ceiling and floor"_hs);
          break;
        case entities::PLAYER:
          if (c.wall.size()) err("player cannot be placed on walls"_hs);
          if (!c.floor.size()) err("player requires ceiling and floor"_hs);
          break;
        case entities::DOOR:
          if (c.wall.size()) err("door cannot be placed on walls"_hs);
          if (!c.floor.size()) err("door requires ceiling and floor"_hs);
          if (c.rotate != 0 && c.rotate != 90) err("door can only be rotated 0 or 90 degrees"_hs);
          break;
        case entities::MOB:
          if (c.rotate) err("mobs can't be rotated yet"_hs);
          if (c.wall.size()) err("mob cannot be placed on walls"_hs);
          if (!c.floor.size()) err("mob requires ceiling and floor"_hs);
          break;
        case entities::NONE:
          if (c.rotate) err("tiles can't be rotated yet"_hs);
          if (c.wall.size()) err("tiles can't have a wall"_hs);
          if (!c.floor.size()) err("tile should have both ceiling and floor"_hs);
          break;
      }
    }
  };

}
