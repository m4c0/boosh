#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import dotz;
import voo;
import vapp;

using vtx = dotz::vec4;

void map_buf(voo::h2l_buffer & buf) {
  unsigned c {};
  voo::memiter<vtx> m { buf.host_memory(), &c };
}

struct : public vapp {
  void run() {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::h2l_buffer buf { dq.physical_device(), sizeof(vtx) * 6 };
      map_buf(buf);

      voo::one_quad quad { dq.physical_device() };

      auto pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings { quad.vertex_input_bind() },
        .attributes { quad.vertex_attribute(0) },
      });

      ots_loop(dq, sw, [&](auto cb) {
        voo::cmd_render_pass rp {{
          .command_buffer = cb,
          .render_pass = dq.render_pass(),
          .framebuffer = sw.framebuffer(),
          .extent = sw.extent(),
        }};
        vee::cmd_set_viewport(cb, sw.extent());
        vee::cmd_set_scissor(cb, sw.extent());
        vee::cmd_bind_gr_pipeline(cb, *gp);
        quad.run(cb, 0, 1);
      });
    });
  }
} t;
