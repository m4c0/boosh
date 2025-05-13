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
    static constexpr const vee::extent extent { mapper::width * 4, mapper::height * 4 };
 
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
    vee::descriptor_set m_ds;
  public:
    fbout(voo::device_and_queue * dq, vee::render_pass::type rp, vee::descriptor_set ds)
      : m_output { dq }
      , m_fb { rp, m_output.image_view() }
      , m_ds { ds }
    {
      vee::update_descriptor_set(m_ds, 0, m_output.image_view(), *v::g->linear_sampler);
    }

    [[nodiscard]] constexpr auto ds() const { return m_ds; }
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
    vee::descriptor_set_layout m_dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler() });
    vee::descriptor_pool m_dpool = vee::create_descriptor_pool(3, { vee::combined_image_sampler(3) });
    vee::descriptor_set m_ds = vee::allocate_descriptor_set(*m_dpool, *m_dsl);
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(*m_dsl, vee::fragment_push_constant_range<vee::extent>());
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
    fbout m_fbout[2];

  public:
    explicit pipeline(voo::device_and_queue * dq, mapper::tilemap * map)
      : m_quad { dq->physical_device() }
      , m_input { dq, map }
      , m_fbout {
        fbout { dq, *m_rp, vee::allocate_descriptor_set(*m_dpool, *m_dsl) },
        fbout { dq, *m_rp, vee::allocate_descriptor_set(*m_dpool, *m_dsl) },
      }
    {
      vee::update_descriptor_set(m_ds, 0, m_input.iv(), *v::g->nearest_sampler);
    }

    [[nodiscard]] constexpr auto output_iv() const { return m_fbout[1].iv(); }

    void run(vee::command_buffer cb) {
      m_input.setup_copy(cb);

      {
        voo::cmd_render_pass rp {{
          .command_buffer = cb,
          .render_pass = *m_rp,
          .framebuffer = m_fbout[1].fb(),
          .extent = output::extent,
        }};

        vee::cmd_bind_gr_pipeline(cb, *m_ppl);
        vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_ds);
        vee::cmd_push_fragment_constants(cb, *m_pl, &output::extent);
        m_quad.run(cb, 0, 1);
      }
      m_fbout[1].cmd_pipeline_barrier(cb);

      for (auto i = 0; i < 16; i++) {
        {
          voo::cmd_render_pass rp {{
            .command_buffer = cb,
            .render_pass = *m_rp,
            .framebuffer = m_fbout[0].fb(),
            .extent = output::extent,
          }};

          vee::cmd_bind_gr_pipeline(cb, *m_ppl);
          vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_fbout[1].ds());
          vee::cmd_push_fragment_constants(cb, *m_pl, &output::extent);
          m_quad.run(cb, 0, 1);
        }
        m_fbout[0].cmd_pipeline_barrier(cb);

        {
          voo::cmd_render_pass rp {{
            .command_buffer = cb,
            .render_pass = *m_rp,
            .framebuffer = m_fbout[1].fb(),
            .extent = output::extent,
          }};

          vee::cmd_bind_gr_pipeline(cb, *m_ppl);
          vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_fbout[0].ds());
          vee::cmd_push_fragment_constants(cb, *m_pl, &output::extent);
          m_quad.run(cb, 0, 1);
        }
        m_fbout[1].cmd_pipeline_barrier(cb);
      }
    }
  };
}

