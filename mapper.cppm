export module mapper;
import jojo;
import jute;

namespace mapper {
  export class loader {
    void (loader::*m_liner)(jute::view) = &loader::take_command;
    unsigned m_line_number = 1;
    jute::view m_filename;
   
    char m_bullet_id;
    char m_hall_id;
    char m_player_id;
    char m_wall_id;

    void check_version(jute::view arg) {
      if (arg != "0") error("invalid version", arg);
    }

    void error(jute::view msg, char c) {
      char cc[2] = { c, 0 };
      error(msg, jute::view { cc, 1 });
    }
    
    void set_id(char * g, jute::view arg) {
      if (arg.size() != 1) error("invalid character id", arg);
      if (g != &m_bullet_id && m_bullet_id == arg[0]) error("id is being used in bullet already", arg);
      if (g != &m_hall_id   && m_hall_id   == arg[0]) error("id is being used in hall already", arg);
      if (g != &m_player_id && m_player_id == arg[0]) error("id is being used in player already", arg);
      if (g != &m_wall_id   && m_wall_id   == arg[0]) error("id is being used in wall already", arg);
      *g = arg[0];
    }
    
    void read_map(jute::view line) {
      if (line == ".") {
        m_liner = &loader::take_command;
        return;
      }
    
      for (auto c : line) {
        if (c == ' ') {}
        else if (c == m_bullet_id) {}
        else if (c == m_hall_id) {}
        else if (c == m_player_id) {}
        else if (c == m_wall_id) {}
        else error("unknown id in map", c);
      }
    }

    void take_command(jute::view line) {
      if (line == "") return;
      
      auto [cmd, args] = line.split(' ');
      args = args.trim();
    
      if (cmd == "version") return check_version(args);
      if (cmd == "bullet")  return set_id(&m_bullet_id, args);
      if (cmd == "hall")    return set_id(&m_hall_id,   args);
      if (cmd == "player")  return set_id(&m_player_id, args);
      if (cmd == "wall")    return set_id(&m_wall_id,   args);
    
      if (cmd == "map") {
        m_liner = &loader::read_map;
        return;
      }
    
      error("unknown command", cmd);
    }

  protected:
    virtual void error(jute::view msg, jute::view arg) = 0;

    auto filename() const { return m_filename; }
    auto line_number() const { return m_line_number; }
    
  public:
    explicit loader(jute::view filename) : m_filename { filename } {
      jojo::readlines(m_filename, [this](auto line) {
        (this->*m_liner)(line.trim());
        m_line_number++;
      });
    }
  };
}
