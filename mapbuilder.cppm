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
      // TODO: optimise walls based on neighbours
      if (d.wall)    draw_wall   (m, x, y, -1, 1, d.wall - 1);
      if (d.floor)   draw_floor  (m, x, y, -1, d.floor   - 1);
      if (d.ceiling) draw_ceiling(m, x, y,  1, d.ceiling - 1);
    });

    return count;
  }
}
