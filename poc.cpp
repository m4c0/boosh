#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import dotz;
import input;
import sitime;
import traits;
import voo;
import vapp;

static constexpr const auto max_vertices = 10240;
static constexpr const auto turn_speed = 180.0f;
static constexpr const auto walk_speed = 5.0f;

struct vtx {
  dotz::vec3 pos;
  dotz::vec2 uv;
};

struct upc {
  dotz::vec3 cam {};
  float angle {};
} g_upc {};

static void draw_ceiling(voo::memiter<vtx> & m, int x, int y, float c) {
  float x0 = x;
  float x1 = x + 1;
  float y0 = y;
  float y1 = y + 1;

  m += vtx { .pos = { x0, c, y0 }, .uv = { 0, 0 } };
  m += vtx { .pos = { x1, c, y0 }, .uv = { 1, 0 } };
  m += vtx { .pos = { x1, c, y1 }, .uv = { 1, 1 } };

  m += vtx { .pos = { x1, c, y1 }, .uv = { 1, 1 } };
  m += vtx { .pos = { x0, c, y1 }, .uv = { 0, 1 } };
  m += vtx { .pos = { x0, c, y0 }, .uv = { 0, 0 } };
}

static void draw_floor(voo::memiter<vtx> & m, int x, int y, float f) {
  float x0 = x;
  float x1 = x + 1;
  float y0 = y;
  float y1 = y + 1;

  m += vtx { .pos = { x0, f, y0 }, .uv = { 0, 0 } };
  m += vtx { .pos = { x1, f, y1 }, .uv = { 1, 1 } };
  m += vtx { .pos = { x1, f, y0 }, .uv = { 1, 0 } };

  m += vtx { .pos = { x1, f, y1 }, .uv = { 1, 1 } };
  m += vtx { .pos = { x0, f, y0 }, .uv = { 0, 0 } };
  m += vtx { .pos = { x0, f, y1 }, .uv = { 0, 1 } };
}

static void draw_x_wall(voo::memiter<vtx> & m, float x0, float x1, float y, float f, float c) {
  m += vtx { .pos = { x0, f, y }, .uv = { 0, 1 } };
  m += vtx { .pos = { x1, f, y }, .uv = { 1, 1 } };
  m += vtx { .pos = { x1, c, y }, .uv = { 1, 0 } };

  m += vtx { .pos = { x1, c, y }, .uv = { 1, 0 } };
  m += vtx { .pos = { x0, c, y }, .uv = { 0, 0 } };
  m += vtx { .pos = { x0, f, y }, .uv = { 0, 1 } };
}

static void draw_y_wall(voo::memiter<vtx> & m, float x, float y0, float y1, float f, float c) {
  m += vtx { .pos = { x, f, y0 }, .uv = { 0, 1 } };
  m += vtx { .pos = { x, f, y1 }, .uv = { 1, 1 } };
  m += vtx { .pos = { x, c, y1 }, .uv = { 1, 0 } };
                              
  m += vtx { .pos = { x, c, y1 }, .uv = { 1, 0 } };
  m += vtx { .pos = { x, c, y0 }, .uv = { 0, 0 } };
  m += vtx { .pos = { x, f, y0 }, .uv = { 0, 1 } };
}

static unsigned g_count {};
static void map_buf(voo::h2l_buffer & buf) {
  voo::memiter<vtx> m { buf.host_memory(), &g_count };
  for (auto y = -10; y < 10; y++) {
    for (auto x = -10; x < 10; x++) {
      draw_ceiling(m, x, y, 1);
      draw_floor(m, x, y, -1);
    }
  }

  for (auto x = -10; x < 10; x++) {
    draw_x_wall(m, x, x + 1, -10, -1, 1);
    draw_x_wall(m, x + 1, x, 9, -1, 1);
  }

  for (auto y = -10; y < 10; y++) {
    draw_y_wall(m, 9, y, y + 1, -1, 1);
    draw_y_wall(m, -10, y + 1, y, -1, 1);
  }
}

static void update_camera(float ms) {
  float da = -input::state(input::axis::TURN) * turn_speed * ms / 1000.0;
  g_upc.angle = dotz::mod(360 + g_upc.angle + da, 360);

  dotz::vec2 dr {
    input::state(input::axis::STRAFE),
    input::state(input::axis::WALK)
  };
  if (dotz::sq_length(dr) == 0) return;

  float a = dotz::radians(g_upc.angle);
  float c = dotz::cos(a);
  float s = dotz::sin(a);
  auto d = dotz::normalise(dr) * walk_speed * ms / 1000.0;

  g_upc.cam.x -= d.x * c - d.y * s;
  g_upc.cam.z += d.x * s + d.y * c;
}

struct : public vapp {
  void run() {
    input::setup();

    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::h2l_buffer buf { dq.physical_device(), sizeof(vtx) * max_vertices };
      map_buf(buf);

      auto pl = vee::create_pipeline_layout({
        vee::vertex_push_constant_range<upc>()
      });
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        //.back_face_cull = false,
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

      sitime::stopwatch time {};
      bool copied = false;
      ots_loop(dq, sw, [&](auto cb) {
        update_camera(time.millis());
        time = {};

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
        vee::cmd_push_vertex_constants(cb, *pl, &g_upc);
        vee::cmd_bind_gr_pipeline(cb, *gp);
        vee::cmd_draw(cb, g_count);
      });
    });
  }
} t;
