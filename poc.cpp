#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"
#pragma leco add_resource "Tiles040_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles051_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles101_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles131_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles133D_1K-JPG_Color.jpg"
#pragma leco add_resource "example.map"

import bullet;
import collision;
import door;
import faces;
import hand;
import input;
import lightmap;
import mapper;
import overlay;
import pushwall;
import silog;
import sires;
import sitime;
import traits;
import v;
import vapp;

using namespace jute::literals;

static constexpr const jute::view map_name = "example.map";

static constexpr const auto turn_speed = 180.0f;
static constexpr const auto walk_speed = 5.0f;

static constexpr const auto player_radius = 0.2f;
static constexpr const auto max_use_dist = 1.0f;

static constexpr const auto dset_smps = 8;

struct upc {
  dotz::vec3 cam {};
  float angle {};
} g_upc {};

dotz::vec4 g_olay {};

static bool update_camera(const mapper::tilemap & map, float ms) {
  float da = -input::state(input::axis::TURN) * turn_speed * ms / 1000.0;
  g_upc.angle = dotz::mod(360 + g_upc.angle + da, 360);

  dotz::vec2 dr {
    input::state(input::axis::STRAFE),
    input::state(input::axis::WALK)
  };
  if (dotz::sq_length(dr) == 0) return false;

  float a = dotz::radians(g_upc.angle);
  float c = dotz::cos(a);
  float s = dotz::sin(a);
  auto d = dotz::normalise(dr);

  const auto walk = [&](auto dx, auto dy) {
    auto cam = g_upc.cam;
    cam.x -= dx * walk_speed * ms / 1000.0;
    cam.z += dy * walk_speed * ms / 1000.0;

    dotz::vec2 cxz { cam.x, cam.z };
    if (collision::bodies().closest(cxz, player_radius).owner) {
      return false;
    }

    g_upc.cam = cam;
    return true;
  };

  auto dx = d.x * c - d.y * s;
  auto dy = d.x * s + d.y * c;
  return walk(dx, dy) || walk(dx, 0) || walk(0, dy);
}

static void process_collisions(auto cb, auto & blt) {
  auto cam = g_upc.cam;
  auto item = collision::entities().closest({ cam.x, cam.z }, player_radius);
  switch (item.owner) {
    case bullet::clid:
      bullet::remove(item.id);
      g_olay = { 0.5f };
      blt.setup_copy(cb);
      break;
  }
  g_olay = g_olay * 0.9;
}

static void process_use() {
  auto cam = g_upc.cam.xz();
  auto angle = dotz::radians(g_upc.angle);
  auto c = collision::entities().hitscan(cam, angle, max_use_dist);
  switch (c.item.owner) {
    case door::clid:
      door::open(c.item.id);
      break;
    case pushwall::clid:
      pushwall::push(cam, c.item.id);
      break;
  }
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
        silog::die("Expecting at least 8 images sampled per descriptor set. Please notify the developer");

      v::globals vg { dq.physical_device() };
      v::g = &vg;

      faces::ceiling ceilings {};
      faces::floor   floors   {};
      faces::wall    walls    {};

      {
        auto c = ceilings.map();
        auto f = floors.map();
        auto w = walls.map();
        map.for_each([&](auto x, auto y, auto & d) {
          // TODO: fix inverted camera Y
          if (*d.entity == "player") g_upc.cam = { x + 0.5f, -0.5f, y + 0.5f };
          if (*d.entity == "bullet") bullet::add({ x + 0.5f, 0.0f, y + 0.5f });
          if (*d.entity == "door") door::add({ x + 0.5f, 0.0f, y + 0.5f }, dotz::radians(d.rotate));

          if (*d.entity == "pushwall") pushwall::add({ x, y }, w.count());
          if (*d.entity == "wall") collision::bodies().add_aabb({ x, y }, { x + 1, y + 1 }, 'wall', 1);

          if (d.wall)    w += { { x, 0, y }, d.wall    - 1 };
          if (d.floor)   f += { { x, 0, y }, d.floor   - 1 };
          if (d.ceiling) c += { { x, 0, y }, d.ceiling - 1 };
        });
      }

      lightmap::pipeline lgm { &map };

      bullet::model  blt  { dq, lgm.output_iv() };
      door::model    dr   { dq, lgm.output_iv() };
      hand::model    hnd  { dq, lgm.output_iv() };
      overlay::model olay { dq, lgm.output_iv() };

      // TODO: refactor to use v::x
      // TODO: move to faces
      auto dsl = vee::create_descriptor_set_layout({
        vee::dsl_fragment_sampler(dset_smps),
        vee::dsl_fragment_sampler(),
      });
      auto pl = vee::create_pipeline_layout(*dsl, vee::vertex_push_constant_range<upc>());
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage("main", vee::specialisation_info<float>(dq.aspect_of())),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings   = faces::bindings(),
        .attributes = faces::attributes(),
      });

      auto dpool = vee::create_descriptor_pool(2, { vee::combined_image_sampler(dset_smps + 1) });
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
      vee::update_descriptor_set(dset, 0, ivs, *v::g->linear_sampler);
      vee::update_descriptor_set(dset, 1, lgm.output_iv(), *v::g->linear_sampler);

      input::on_button_down(input::buttons::ATTACK, hand::attack);
      input::on_button_down(input::buttons::USE,    process_use);

      sitime::stopwatch time {};
      bool copied = false;
      ots_loop(dq, sw, [&](auto cb) {
        // TODO: add a frame time limit or time interpolation
        bool moved = update_camera(map, time.millis());
        process_collisions(cb, blt);
        // TODO: squish
        pushwall::tick(walls, time.millis());
        dr.tick(time.millis());
        hnd.tick(time.millis(), moved, g_upc.cam, g_upc.angle);
        time = {};

        if (!copied) {
          ceilings.setup_copy(cb);
          floors.setup_copy(cb);
          blt.setup_copy(cb);
          dr.setup_copy(cb);
          for (auto &i : imgs) i.setup_copy(cb);
          copied = true;
        }
        lgm.run(cb);
        walls.setup_copy(cb);
        dr.copy_models(cb);

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

        dr.draw(cb, g_upc.cam, g_upc.angle);
        blt.draw(cb, g_upc.cam, g_upc.angle);

        hnd.run(cb);
        olay.run(cb, g_olay);
      });
    });
  } catch (const mapper::error & e) {
    silog::die("%s", (*e.msg).cstr().begin());
  }
} t;
