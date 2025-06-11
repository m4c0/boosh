export module mapper:loader;
import :error;
import :tiledefs;
import :tilemap;
import :textures;
import jojo;
import jute;
import hai;
import traits;

using namespace jute::literals;

namespace mapper {
  class loader {
    void (loader::*m_liner)(jute::view) = &loader::take_command;
    unsigned m_line_number = 1;
    unsigned m_map_row = 1;
   
    tiledefs m_tdefs {};
    tilemap m_map {};

    void cmd_version(jute::view arg) {
      arg = arg.trim();
      if (arg != "0") throw error { "invalid version: "_hs + arg };
    }

    void cmd_define(jute::view arg) {
      m_tdefs.add(arg);
      m_liner = &loader::read_define;
    }

    void read_define(jute::view line) {
      line = line.trim();
      if (line == ".") {
        m_tdefs.validate_last();
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
        if (c != ' ') m_map(x, y) = m_tdefs[c];
      }
    }

    void take_command(jute::view line) {
      line = line.trim();
      if (line == "") return;
      if (line.subview(2).before == "//") return;
      
      auto [cmd, args] = line.split(' ');
    
      if (cmd == "version") return cmd_version(args);
      if (cmd == "define")  return cmd_define(args);

      if (cmd == "map") {
        m_liner = &loader::read_map;
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

    [[nodiscard]] constexpr auto take() {
      return traits::move(m_map);
    }
  };
}
