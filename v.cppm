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

  export struct globals {
    voo::device_and_queue * dq;

    vee::physical_device pd = dq->physical_device();

    vee::sampler linear_sampler = vee::create_sampler(vee::linear_sampler);
    vee::sampler nearest_sampler = vee::create_sampler(vee::nearest_sampler);

    voo::single_dset lightmap { 
      vee::dsl_fragment_samplers({ *linear_sampler }),
      vee::combined_image_sampler(),
    };
    vee::descriptor_set uber_set;

    camera camera {};

    explicit globals(voo::device_and_queue * dq) : dq { dq } {}
  };
  export globals * g;
}
