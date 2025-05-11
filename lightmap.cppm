#pragma leco add_shader "lightmap.vert"
#pragma leco add_shader "lightmap.frag"
export module lightmap;
import mapper;
import v;

namespace lightmap {
  static constexpr const auto rgba_fmt = VK_FORMAT_R8G8B8A8_UNORM;

  class input : voo::h2l_image {
  public:
    explicit input(voo::device_and_queue * dq, mapper::tilemap * map)
      : voo::h2l_image { dq->physical_device(), mapper::width, mapper::height, rgba_fmt }
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

  class output : voo::offscreen::colour_buffer {
  public:
    static constexpr const vee::extent extent { mapper::width * 2, mapper::height * 2 };
 
    explicit output(voo::device_and_queue * dq)
      : colour_buffer { dq->physical_device(), extent, rgba_fmt, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
    {}

    using colour_buffer::image_view;
    using colour_buffer::image;
  };

  class framebuffer : vee::framebuffer {
  public:
    framebuffer(vee::render_pass::type rp, vee::image_view::type iv)
      : vee::framebuffer { vee::create_framebuffer({
        .render_pass = rp,
        .attachments = {{ iv }},
        .extent = output::extent,
      }) }
    {}

    constexpr operator vee::framebuffer::type() const { return **this; }
  };

  class fbout {
    output m_output;
    framebuffer m_fb;
  public:
    fbout(voo::device_and_queue * dq, vee::render_pass::type rp)
      : m_output { dq }
      , m_fb { rp, m_output.image_view() }
    {}

    [[nodiscard]] constexpr auto & fb() const { return m_fb; }
    [[nodiscard]] constexpr auto iv() const { return m_output.image_view(); }

    void cmd_pipeline_barrier(vee::command_buffer cb) {
      constexpr const auto stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      vee::cmd_pipeline_barrier(cb, stage, stage, {
        .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
        .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .image = m_output.image(),
      });
    }
  };

  export class pipeline {
    v::linear_sampler m_smp {};
    voo::single_frag_dset m_ds { 1 };
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(
      m_ds.descriptor_set_layout()
    );
    vee::render_pass m_rp = vee::create_render_pass({
      .attachments {{
        vee::create_colour_attachment(rgba_fmt, vee::image_layout_color_attachment_optimal),
      }},
      .subpasses {{
        vee::create_subpass({
          .colours {{ vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal) }},
        }),
      }},
      .dependencies {{
        vee::create_colour_dependency(),
      }},
    });
    vee::gr_pipeline m_ppl = vee::create_graphics_pipeline({
      .pipeline_layout = *m_pl,
      .render_pass = *m_rp,
      .extent = output::extent,
      .shaders {
        voo::shader("lightmap.vert.spv").pipeline_vert_stage(),
        voo::shader("lightmap.frag.spv").pipeline_frag_stage(),
      },
      .bindings { voo::one_quad::vertex_input_bind() },
      .attributes { voo::one_quad::vertex_attribute(0) },
    });

    voo::one_quad m_quad; 
    input m_input;
    fbout m_fbout;

  public:
    explicit pipeline(voo::device_and_queue * dq, mapper::tilemap * map)
      : m_quad { dq->physical_device() }
      , m_input { dq, map }
      , m_fbout { dq, *m_rp }
    {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, m_input.iv(), m_smp);
    }

    [[nodiscard]] constexpr auto output_iv() const { return m_fbout.iv(); }

    void run(vee::command_buffer cb) {
      m_input.setup_copy(cb);

      {
        voo::cmd_render_pass rp {{
          .command_buffer = cb,
          .render_pass = *m_rp,
          .framebuffer = m_fbout.fb(),
          .extent = output::extent,
        }};

        vee::cmd_bind_gr_pipeline(cb, *m_ppl);
        vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_ds.descriptor_set());
        m_quad.run(cb, 0, 1);
      }

      m_fbout.cmd_pipeline_barrier(cb);
    }
  };
}

