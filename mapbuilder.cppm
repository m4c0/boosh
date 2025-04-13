export module mapbuilder;
import dotz;
import faces;
import mapper;
import voo;

using namespace mapper;

namespace mapbuilder {
  export unsigned build(const mapper::tilemap & map, voo::h2l_buffer & buf) {
    unsigned count {};
    voo::memiter<faces::vtx> m { buf.host_memory(), &count };

    map.for_each([&](auto x, auto y, auto & d) {
      if (d.wall) {
        // TODO: optimise walls based on neighbours
        draw_x_wall(m, x,     x + 1, y + 1, -1, 1, d.wall - 1);
        draw_x_wall(m, x + 1, x,     y,     -1, 1, d.wall - 1);
        draw_y_wall(m, x,     y,     y + 1, -1, 1, d.wall - 1);
        draw_y_wall(m, x + 1, y + 1, y,     -1, 1, d.wall - 1);
      }
      if (d.floor)   draw_floor  (m, x, y, -1, d.floor   - 1);
      if (d.ceiling) draw_ceiling(m, x, y,  1, d.ceiling - 1);
    });

    return count;
  }
}
