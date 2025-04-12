export module mapper;
import bullet;
import dotz;
import hai;
import jojo;
import jute;

namespace mapper {
  export dotz::ivec2 initial_pos { -1 };

  export constexpr const auto width = 256;
  export constexpr const auto height = 256;
  export enum class tile { empty, hall, wall };
  export tile tiles[height][width];

  static void ignore(unsigned, unsigned) {}
  static void add_bullet(unsigned x, unsigned y) {
    bullet::add({ x + 0.5f, 0.0f, y + 0.5f });
    tiles[y][x] = tile::hall;
  }
  static void add_hall(unsigned x, unsigned y) {
    tiles[y][x] = tile::hall;
  }
  static void add_player(unsigned x, unsigned y) {
    initial_pos = { x, y };
    tiles[y][x] = tile::hall;
  }
  static void add_wall(unsigned x, unsigned y) {
    tiles[y][x] = tile::wall;
  }

  export class loader {
    void (loader::*m_liner)(jute::view) = &loader::take_command;
    unsigned m_line_number = 1;
    unsigned m_map_row = 1;
   
    hai::fn<void, int, int> m_fns[256] {};

    void check_version(jute::view arg) {
      arg = arg.trim();
      if (arg != "0") err("invalid version", arg);
    }

    void err(jute::view msg, char c) {
      char cc[2] = { c, 0 };
      err(msg, jute::view { cc, 1 });
    }
    
    void set_id(jute::view arg, auto fn) {
      arg = arg.trim();
      if (arg.size() != 1) err("invalid character id", arg);
      
      int id = arg[0];
      if (m_fns[id]) err("id is being used already", arg);
      m_fns[id] = fn;
    }
    
    void read_map(jute::view line) {
      if (line == ".") {
        m_liner = &loader::take_command;
        return;
      }

      unsigned x = 0;
      unsigned y = m_map_row++;
      for (auto c : line) {
        x++;
        auto & fn = m_fns[static_cast<unsigned>(c)];
        if (fn) fn(x, y);
        else err("unknown id in map", c);
      }
    }

    void take_command(jute::view line) {
      line = line.trim();
      if (line == "") return;
      
      auto [cmd, args] = line.split(' ');
    
      if (cmd == "version") return check_version(args);
      if (cmd == "bullet")  return set_id(args, &add_bullet);
      if (cmd == "hall")    return set_id(args, &add_hall);
      if (cmd == "player")  return set_id(args, &add_player);
      if (cmd == "wall")    return set_id(args, &add_wall);
    
      if (cmd == "map") {
        bullet::clear();
        m_liner = &loader::read_map;

        for (auto & row: tiles) for (auto &t: row) t = {};
        m_map_row = 1;
        return;
      }
    
      err("unknown command", cmd);
    }

    [[noreturn]] void err(jute::view msg, jute::view arg) {
      auto m = jute::heap { msg } + " [" + arg + "]";
      throw error { m, m_line_number };
    }

  public:
    struct error {
      jute::heap msg;
      unsigned line_number;
    };

    explicit loader(jute::view filename) {
      m_fns[' '] = &ignore;
      jojo::readlines(filename, [this](auto line) {
        (this->*m_liner)(line);
        m_line_number++;
      });
    }
  };
}
