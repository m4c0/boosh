#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"
#pragma leco add_shader "overlay.vert"
#pragma leco add_shader "overlay.frag"
#pragma leco add_resource "Tiles040_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles051_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles101_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles131_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles133D_1K-JPG_Color.jpg"
#pragma leco add_resource "example.map"

import bullet;
import collision;
import dotz;
import faces;
import hai;
import jute;
import input;
import mapper;
import mapper;
import silog;
import sires;
import sitime;
import traits;
import voo;
import vapp;

using namespace jute::literals;

static constexpr const jute::view map_name = "example.map";

static constexpr const auto turn_speed = 180.0f;
static constexpr const auto walk_speed = 5.0f;

static constexpr const auto player_radius = 0.2f;

static constexpr const auto dset_smps = 16;

struct upc {
  dotz::vec3 cam {};
  float angle {};
} g_upc {};

dotz::vec4 g_olay {};

static void update_camera(const mapper::tilemap & map, float ms) {
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
  auto d = dotz::normalise(dr);

  const auto walk = [&](auto dx, auto dy) {
    auto cam = g_upc.cam;
    cam.x -= dx * walk_speed * ms / 1000.0;
    cam.z += dy * walk_speed * ms / 1000.0;
    if (!map(cam.x, cam.z).walk) return false;

    auto adx = player_radius * dotz::sign(-dx);
    auto ady = player_radius * dotz::sign(dy);
    if (!map(cam.x + adx, cam.z      ).walk) return false;
    if (!map(cam.x,       cam.z + ady).walk) return false;
    if (!map(cam.x + adx, cam.z + ady).walk) return false;

    g_upc.cam = cam;
    return true;
  };

  auto dx = d.x * c - d.y * s;
  auto dy = d.x * s + d.y * c;
  walk(dx, dy) || walk(dx, 0) || walk(0, dy);
}

static void process_collisions() {
  auto cam = g_upc.cam;
  auto item = collision::entities().closest({ cam.x, cam.z }, player_radius);
  switch (item.type) {
    case bullet::clid: break;
  }
}

static void process_pickups(auto cb, auto & blt) {
  if (bullet::take(g_upc.cam)) {
    g_olay = { 1.0f };
    blt.setup_copy(cb);
  }
  g_olay = g_olay * 0.9;
}

struct : public vapp {
  void run() try {
    input::setup();
    auto [map, textures] = mapper::load(sires::real_path_name(map_name));

    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      unsigned max_dset_imgs = vee::get_physical_device_properties(dq.physical_device())
        .limits
        .maxDescriptorSetSampledImages;
      silog::log(silog::info, "Max descriptor set sampled images: %u", max_dset_imgs);
      if (max_dset_imgs < dset_smps)
        silog::die("Expecting at least 16 images sampled per descriptor set. Please notify the developer");

      faces::ceiling ceilings { dq.physical_device() };
      faces::floor   floors   { dq.physical_device() };
      faces::wall    walls    { dq.physical_device() };

      {
        auto c = ceilings.map();
        auto f = floors.map();
        auto w = walls.map();
        map.for_each([&](auto x, auto y, auto & d) {
          if (*d.entity == "player") g_upc.cam = { x + 0.5f, 0.0f, y + 0.5f };
          if (*d.entity == "bullet") bullet::add({ x + 0.5f, 0.0f, y + 0.5f });

          if (d.wall)    w += { { x, 0, y }, d.wall    - 1 };
          if (d.floor)   f += { { x, 0, y }, d.floor   - 1 };
          if (d.ceiling) c += { { x, 0, y }, d.ceiling - 1 };
        });
      }

      bullet::model blt { dq };

      auto dsl = vee::create_descriptor_set_layout({
        vee::dsl_fragment_sampler(dset_smps)
      });
      auto pl = vee::create_pipeline_layout(*dsl, vee::vertex_push_constant_range<upc>());
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        //.back_face_cull = false,
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings   = faces::bindings(),
        .attributes = faces::attributes(),
      });

      auto smp = vee::create_sampler(vee::linear_sampler);
      auto dpool = vee::create_descriptor_pool(1, { vee::combined_image_sampler(dset_smps) });
      auto dset = vee::allocate_descriptor_set(*dpool, *dsl);

      hai::array<voo::h2l_image> imgs { textures.size() };
      hai::array<vee::image_view::type> ivs { dset_smps };
      for (auto i = 0; i < imgs.size(); i++) {
        imgs[i] = voo::load_sires_image(*textures[i], dq.physical_device());
        ivs[i] = imgs[i].iv();
      }
      for (auto i = imgs.size(); i < dset_smps; i++) {
        ivs[i] = imgs[0].iv();
      }
      vee::update_descriptor_set(dset, 0, ivs, *smp);

      auto opl = vee::create_pipeline_layout({
        vee::fragment_push_constant_range<dotz::vec4>()
      });
      voo::one_quad_render oqr { "overlay", &dq, *opl };

      sitime::stopwatch time {};
      bool copied = false;
      ots_loop(dq, sw, [&](auto cb) {
        update_camera(map, time.millis());
        process_collisions();
        process_pickups(cb, blt);
        time = {};

        if (!copied) {
          ceilings.setup_copy(cb);
          floors.setup_copy(cb);
          walls.setup_copy(cb);
          blt.setup_copy(cb);
          for (auto &i : imgs) i.setup_copy(cb);
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
        vee::cmd_push_vertex_constants(cb, *pl, &g_upc);
        vee::cmd_bind_gr_pipeline(cb, *gp);
        vee::cmd_bind_descriptor_set(cb, *pl, 0, dset);
        ceilings.draw(cb);
        floors.draw(cb);
        walls.draw(cb);

        blt.draw(cb, g_upc.cam, g_upc.angle);

        vee::cmd_push_fragment_constants(cb, *opl, &g_olay);
        oqr.run(cb);
      });
    });
  } catch (const mapper::error & e) {
    silog::die("%s", (*e.msg).cstr().begin());
  }
} t;
