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

    camera camera {};

    explicit globals(voo::device_and_queue * dq) : dq { dq } {}
  };
  export globals * g;
}
