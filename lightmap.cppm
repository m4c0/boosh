export module lightmap;
import mapper;
import v;

namespace lightmap {
  export class input : voo::h2l_image {
  public:
    explicit input(voo::device_and_queue * dq, mapper::tilemap * map)
      : voo::h2l_image { dq->physical_device(), mapper::width, mapper::height, VK_FORMAT_R8G8B8A8_UNORM }
    {
      struct pix { char lvl; char trns; char pad[2]; };
      map->for_each([m = voo::memiter<pix>(host_memory())](auto x, auto y, auto tile) mutable {
        m[y * mapper::width + x] = {
          .lvl  = *tile.entity == "" ? '\x00' : '\xFF',
          .trns = !tile.ceiling ? '\x00' : '\xFF',
        };
      });
    }

    using voo::h2l_image::iv;
    using voo::h2l_image::setup_copy;
  };

  export class texture {
    voo::offscreen::colour_buffer m_img;

  public:
    explicit texture(voo::device_and_queue * dq)
      : m_img { dq->physical_device(), { 1024, 1024 }, VK_FORMAT_R8G8B8A8_UNORM }
    {}

    [[nodiscard]] auto image_view() const { return m_img.image_view(); }
  };
}

