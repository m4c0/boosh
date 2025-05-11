#pragma leco add_shader "lightmap.vert"
#pragma leco add_shader "lightmap.frag"
export module lightmap;
import mapper;
import v;

namespace lightmap {
  static constexpr const auto rgba_fmt = VK_FORMAT_R8G8B8A8_UNORM;

  export class input : voo::h2l_image {
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

  export class output : voo::offscreen::colour_buffer {
  public:
    explicit output(voo::device_and_queue * dq)
      : colour_buffer { dq->physical_device(), { mapper::width * 2, mapper::height * 2 }, rgba_fmt }
    {}

    using colour_buffer::image_view;
  };

  export class pipeline {
    vee::sampler m_smp = vee::create_sampler(vee::linear_sampler);
    voo::single_frag_dset m_ds { 2 };
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
      .shaders {
        voo::shader("lightmape.vert.spv").pipeline_vert_stage(),
        voo::shader("lightmape.frag.spv").pipeline_frag_stage(),
      },
      .bindings { voo::one_quad::vertex_input_bind() },
      .attributes { voo::one_quad::vertex_attribute(0) },
    });

    voo::one_quad m_quad; 

  public:
    explicit pipeline(voo::device_and_queue * dq)
      : m_quad { dq->physical_device() }
    {}

    void run(vee::command_buffer cb) {
    }
  };

}

