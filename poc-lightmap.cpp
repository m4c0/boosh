#pragma leco app
#pragma leco add_shader "poc-lightmap.frag"
#pragma leco add_shader "poc-lightmap.vert"
#pragma leco add_resource "example.map"

import casein;
import lightmap;
import mapper;
import sires;
import vapp;
import v;

struct app : public vapp {
  app() {
    casein::window_size = { 800 };
  }
  void run() override {
    main_loop("poc-lightmap", [&](auto & dq, auto & sw) {
      v::globals vg { &dq };
      v::g = &vg;

      auto [map, textures] = mapper::load(sires::real_path_name("example.map"));
      lightmap::pipeline ppl { &map };

      voo::single_frag_dset ds { 1 };
      vee::update_descriptor_set(ds.descriptor_set(), 0, ppl.output_iv(), *v::g->nearest_sampler);

      auto pl = vee::create_pipeline_layout(ds.descriptor_set_layout());
      voo::one_quad_render oqr { "poc-lightmap", &dq, *pl };
      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          ppl.run(*pcb);

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
