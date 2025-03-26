#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import dotz;
import voo;
import vapp;

using vtx = dotz::vec4;

unsigned g_count {};
void map_buf(voo::h2l_buffer & buf) {
  voo::memiter<vtx> m { buf.host_memory(), &g_count };
  //m += vtx { -1.f, -1.f, 0.9f, 1.f };
  //m += vtx { +1.f, +1.f, 0.9f, 1.f };
  //m += vtx { +1.f, -1.f, 0.9f, 1.f };

  m += vtx { -1.f, -1.f, -9.9f, 1.f };
  m += vtx { +1.f, +1.f, -9.9f, 1.f };
  m += vtx { +1.f, -1.f, -9.9f, 1.f };

  m += vtx { -10, -1, -10, 1 };
  m += vtx { +10, -1, +10, 1 };
  m += vtx { +10, -1, -10, 1 };

  m += vtx { +10, -1, +10, 1 };
  m += vtx { -10, -1, -10, 1 };
  m += vtx { -10, -1, +10, 1 };

  m += vtx { -10, +1, -10, 1 };
  m += vtx { +10, +1, +10, 1 };
  m += vtx { +10, +1, -10, 1 };

  m += vtx { +10, +1, +10, 1 };
  m += vtx { -10, +1, -10, 1 };
  m += vtx { -10, +1, +10, 1 };
}

struct : public vapp {
  void run() {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::h2l_buffer buf { dq.physical_device(), sizeof(vtx) * 128 };
      map_buf(buf);

      auto pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        .back_face_cull = false,
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec4(0, 0),
        },
      });

      bool copied = false;
      ots_loop(dq, sw, [&](auto cb) {
        if (!copied) {
          buf.setup_copy(cb);
          copied = true;
        }

        voo::cmd_render_pass rp {{
          .command_buffer = cb,
          .render_pass = dq.render_pass(),
          .framebuffer = sw.framebuffer(),
          .extent = sw.extent(),
        }};
        vee::cmd_set_viewport(cb, sw.extent());
        vee::cmd_set_scissor(cb, sw.extent());
        vee::cmd_bind_vertex_buffers(cb, 0, buf.local_buffer());
        vee::cmd_bind_gr_pipeline(cb, *gp);
        vee::cmd_draw(cb, g_count);
      });
    });
  }
} t;
