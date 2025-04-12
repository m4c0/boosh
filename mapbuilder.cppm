export module mapbuilder;
import dotz;
import faces;
import mapper;
import voo;

using namespace mapper;

namespace mapbuilder {
  export bool walkable(unsigned x, unsigned y) {
    if (x <= 0 || x >= width || y <= 0 || y >= height) return false;
    return tiles[y][x] == tile::hall;
  }

  export unsigned load(voo::h2l_buffer & buf) {
    unsigned count {};
    voo::memiter<faces::vtx> m { buf.host_memory(), &count };

    for (auto y = 0; y < height; y++) {
      for (auto x = 0; x < width; x++) {
        switch (tiles[y][x]) {
          case tile::wall:
            // TODO: make walls based on neighbours
            draw_x_wall(m, x, x + 1, y + 1, -1, 1, 0);
            draw_x_wall(m, x + 1, x, y, -1, 1, 0);
            draw_y_wall(m, x, y, y + 1, -1, 1, 0);
            draw_y_wall(m, x + 1, y + 1, y, -1, 1, 0);
            break;

          case tile::hall:
            draw_floor(m, x, y, -1, 3);
            draw_ceiling(m, x, y, 1, 4);
            break;

          default:
            break;
        }
      }
    }

    return count;
  }
}
