export module mapper;
export import :error;
export import :tilemap;
import :loader;
import jute;

namespace mapper {
  export auto load(jute::view name) {
    return loader { name }.take_map();
  }
}
