export module v;
export import dotz;
export import hai;
export import jute;
export import voo;
import silog;
import wagen;

namespace v {
  static constexpr const auto dset_smps = 8;

  export struct camera {
    dotz::vec3 cam {};
    float angle {};
  };

  export struct globals {
    voo::device_and_queue * dq;

    vee::physical_device pd = dq->physical_device();

    vee::sampler linear_sampler = vee::create_sampler(vee::linear_sampler);
    vee::sampler nearest_sampler = vee::create_sampler(vee::nearest_sampler);

    voo::single_frag_dset lightmap { 1 };

    camera camera {};

    explicit globals(voo::device_and_queue * dq) : dq { dq } {
      unsigned max_dset_imgs = vee::get_physical_device_properties(pd)
        .limits
        .maxPerStageDescriptorSamplers;

      silog::log(silog::info, "Max per-stage descriptor set sampled images: %u", max_dset_imgs);
      if (max_dset_imgs < dset_smps)
        silog::die("Expecting at least %d images sampled per stage per descriptor set. Please notify the developer", dset_smps);
    }
  };
  export globals * g;
}
