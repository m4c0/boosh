export module lightmap;
import mapper;
import v;

namespace lightmap {
  constexpr const auto width = mapper::width;
  constexpr const auto height = mapper::height;

  class input {
    voo::h2l_image m_txt;

  public:
    explicit input(voo::device_and_queue * dq)
      : m_txt { dq->physical_device(), width, height, VK_FORMAT_R8G8B8A8_UNORM }
    {}
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

