export module mapper;
export import :error;
import :tiledefs;
import :textures;
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
  export tiledef tiles2[height][width];

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
   
    textures m_txts {};
    tiledefs m_tdefs { &m_txts };

    void cmd_version(jute::view arg) {
      arg = arg.trim();
      if (arg != "0") throw error { "invalid version: "_hs + arg };
    }

    void cmd_texture(jute::view arg) { m_txts.add(arg); }

    void cmd_define(jute::view arg) {
      m_tdefs.add(arg);
      m_liner = &loader::read_define;
    }

    void read_define(jute::view line) {
      line = line.trim();
      if (line == ".") {
        m_liner = &loader::take_command;
        return;
      }
      m_tdefs.parse(line);
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
        if (c != ' ') tiles2[y][x] = m_tdefs[c];
      }
    }

    void take_command(jute::view line) {
      line = line.trim();
      if (line == "") return;
      if (line.subview(2).before == "//") return;
      
      auto [cmd, args] = line.split(' ');
    
      if (cmd == "version") return cmd_version(args);
      if (cmd == "texture") return cmd_texture(args);
      if (cmd == "define")  return cmd_define(args);

      if (cmd == "map") {
        bullet::clear();
        m_liner = &loader::read_map;

        for (auto & row: tiles) for (auto &t: row) t = {};
        m_map_row = 1;
        return;
      }
    
      throw error { "unknown command: "_hs + cmd };
    }

  public:
    explicit loader(jute::view filename) {
      try {
        jojo::readlines(filename, [this](auto line) {
          (this->*m_liner)(line);
          m_line_number++;
        });
      } catch (error e) {
        e.line_number = m_line_number;
        throw e;
      }
    }
  };
}
