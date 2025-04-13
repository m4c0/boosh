export module mapper;
export import :error;
export import :loader;
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
  export tile tiles[width][height];
}
