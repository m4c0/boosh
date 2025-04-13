export module mapper;
export import :error;
import :loader;
import jute;

namespace mapper {
  export constexpr const auto width = 256;
  export constexpr const auto height = 256;
  export enum class tile { empty, hall, wall };
  export tile tiles[width][height];

  export auto load(jute::view name) {
    return loader { name }.take_map();
  }
}
