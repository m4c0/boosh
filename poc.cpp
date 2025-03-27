#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import dotz;
import traits;
import voo;
import vapp;

static constexpr const auto max_vertices = 10240;

struct vtx {
  dotz::vec3 pos;
  dotz::vec2 uv;
};

static unsigned g_count {};
static void map_buf(voo::h2l_buffer & buf) {
  voo::memiter<vtx> m { buf.host_memory(), &g_count };

  m += vtx { .pos = { -10, +1, -10 }, .uv = { 0, 1 } };
  m += vtx { .pos = { +10, +1, -10 }, .uv = { 0, 1 } };
  m += vtx { .pos = { +10, +1, +10 }, .uv = { 0, 1 } };

  m += vtx { .pos = { +10, +1, +10 }, .uv = { 0, 1 } };
  m += vtx { .pos = { -10, +1, +10 }, .uv = { 0, 1 } };
  m += vtx { .pos = { -10, +1, -10 }, .uv = { 0, 1 } };

  m += vtx { .pos = { -10, -1, -10 }, .uv = { 1, 0 } };
  m += vtx { .pos = { +10, -1, +10 }, .uv = { 1, 0 } };
  m += vtx { .pos = { +10, -1, -10 }, .uv = { 1, 0 } };

  m += vtx { .pos = { +10, -1, +10 }, .uv = { 1, 0 } };
  m += vtx { .pos = { -10, -1, -10 }, .uv = { 1, 0 } };
  m += vtx { .pos = { -10, -1, +10 }, .uv = { 1, 0 } };
}

struct : public vapp {
  void run() {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::h2l_buffer buf { dq.physical_device(), sizeof(vtx) * max_vertices };
      map_buf(buf);

      auto pl = vee::create_pipeline_layout();
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::uv)),
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
