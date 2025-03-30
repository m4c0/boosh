export module mapbuilder;
import dotz;
import faces;
import voo;

static constexpr const auto width = 12;
static constexpr const auto height = 15;
static constexpr const char raw[] =
  "            "
  " XXXXX  XXX "
  " X...X X..X "
  " X....X...X "
  " X........X "
  " X.P......X "
  " X........X "
  " XXX....XXX "
  "   X....X   "
  "   X..XXX   "
  "   X..X     "
  "   X.XX     "
  "   X.X      "
  "   XXX      "
  "            ";
static_assert(sizeof(raw) == width * height + 1);

namespace mapbuilder {
  export dotz::ivec2 initial_pos() {
    for (dotz::ivec2 p {}; p.y < height; p.y++) {
      for (p.x = 0; p.x < width; p.x++) {
        if (raw[p.y * width + p.x] == 'P') return p;
      }
    }
    return {};
  }

  export bool walkable(unsigned x, unsigned y) {
    if (x <= 0 || x >= width || y <= 0 || y >= height) return false;
    switch (raw[y * width + x]) {
      case '.':
      case 'P': return true;
      default:  return false;
    }
  }

  export unsigned load(voo::h2l_buffer & buf) {
    unsigned count {};
    voo::memiter<faces::vtx> m { buf.host_memory(), &count };

    for (auto y = 0; y < height; y++) {
      for (auto x = 0; x < width; x++) {
        switch (raw[y * width + x]) {
          case 'X':
            // TODO: make walls based on neighbours
            draw_x_wall(m, x, x + 1, y + 1, -1, 1, 0);
            draw_x_wall(m, x + 1, x, y, -1, 1, 0);
            draw_y_wall(m, x, y, y + 1, -1, 1, 0);
            draw_y_wall(m, x + 1, y + 1, y, -1, 1, 0);
            break;

          case '.':
          case 'P':
            draw_floor(m, x, y, -1, 2);
            draw_ceiling(m, x, y, 1, 3);
            break;
        }
      }
    }

    return count;
  }
}
