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
}
