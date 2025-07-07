export module mapper;
export import :error;
export import :tilemap;
export import :loader;
import jute;

namespace mapper {
  export auto load(jute::view name) {
    unsigned line_number = 1;
    try {
      loader l {};
      jojo::readlines(name, [&](auto line) {
        l.parse(line);
        line_number++;
      });
      return l.take();
    } catch (error & e) {
      e.line_number = line_number;
      throw traits::move(e);
    }
  }
}
