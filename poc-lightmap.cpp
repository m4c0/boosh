#pragma leco app
#pragma leco add_shader "poc-lightmap.frag"
#pragma leco add_shader "poc-lightmap.vert"

import lightmap;
import vee;
import voo;
import vapp;

struct app : public vapp {
  void run() override {
    main_loop("poc-lightmap", [&](auto & dq, auto & sw) {
      lightmap::texture txt { &dq };

      auto smp = vee::create_sampler(vee::linear_sampler);

      voo::single_frag_dset ds { 1 };
      vee::update_descriptor_set(ds.descriptor_set(), 0, txt.image_view(), *smp);

      auto pl = vee::create_pipeline_layout(ds.descriptor_set_layout());
      voo::one_quad_render oqr { "poc-lightmap", &dq, *pl };
      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass({ *pcb });
          vee::cmd_set_viewport(*pcb, sw.extent());
          vee::cmd_set_scissor(*pcb, sw.extent());
          vee::cmd_bind_descriptor_set(*pcb, *pl, 0, ds.descriptor_set());
          oqr.run(*pcb);
        });
      });
    });
  }
} t;
