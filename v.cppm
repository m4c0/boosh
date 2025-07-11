export module v;
export import dotz;
export import hai;
export import jute;
export import voo;
import silog;
import wagen;

namespace v {
  export struct camera {
    dotz::vec3 cam {};
    float angle {};
  };

  export struct grpl {
    vee::pipeline_layout layout;
    vee::gr_pipeline pipeline;
  };

  export constexpr const auto uber_dset_smps = 16;

  export struct globals {
    voo::device_and_queue * dq;

    vee::physical_device pd = dq->physical_device();

    vee::sampler linear_sampler = vee::create_sampler(vee::linear_sampler);
    vee::sampler nearest_sampler = vee::create_sampler(vee::nearest_sampler);

    voo::single_dset lightmap { 
      vee::dsl_fragment_samplers({ *linear_sampler }),
      vee::combined_image_sampler(),
    };
    voo::single_dset uber_set {
      vee::dsl_fragment_samplers([this] {
        hai::array<vee::sampler::type> res { uber_dset_smps };
        for (auto & s : res) s = *linear_sampler;
        return res;
      }()),
      vee::combined_image_sampler(uber_dset_smps),
    };
    vee::render_pass rp { vee::create_render_pass({
      .attachments {{
        vee::create_colour_attachment(dq->physical_device(), dq->surface()),
        vee::create_depth_attachment(),
      }},
      .subpasses {{
        vee::create_subpass({
          .colours {{ vee::create_attachment_ref(0, vee::image_layout_color_attachment_optimal) }},
          .depth_stencil = vee::create_attachment_ref(1, vee::image_layout_depth_stencil_attachment_optimal),
        }),
      }},
      .dependencies {{
        vee::create_colour_dependency(),
        vee::create_depth_dependency(),
      }},
    }) };

    grpl model_pipeline {};

    camera camera {};

    explicit globals(voo::device_and_queue * dq) : dq { dq } {}
  };
  export globals * g;
}
