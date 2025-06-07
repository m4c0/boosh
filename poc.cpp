#pragma leco app
#pragma leco add_resource "Tiles040_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles051_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles101_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles131_1K-JPG_Color.jpg"
#pragma leco add_resource "Tiles133D_1K-JPG_Color.jpg"
#pragma leco add_resource "example.map"

import bullet;
import camera;
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

static constexpr const auto player_radius = 0.2f;
static constexpr const auto max_use_dist = 1.0f;

dotz::vec4 g_olay {};

static void process_collisions(auto cb, auto & blt) {
  auto cam = v::g->camera.cam;
  auto item = collision::entities().closest({ cam.x, cam.z }, player_radius);
  switch (item.owner) {
    case bullet::clid:
      blt.remove(item.id);
      blt.setup_copy(cb);
      g_olay = { 0.5f };
      break;
  }
  g_olay = g_olay * 0.9;
}

static void process_use(door::model * dr, pushwall::model * psh) {
  auto cam = v::g->camera.cam.xz();
  auto angle = dotz::radians(v::g->camera.angle);
  auto c = collision::entities().hitscan(cam, angle, max_use_dist);
  switch (c.item.owner) {
    case door::clid:
      dr->open(c.item.id);
      break;
    case pushwall::clid:
      psh->push(c.item.id);
      break;
  }
}

struct : public vapp {
  void run() try {
    input::setup();
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      v::globals vg { &dq };
      v::g = &vg;

      lightmap::pipeline lgm {};

      faces::model    faces {};
      bullet::model   blt   {};
      door::model     dr    {};
      pushwall::model psh   {};
      hand::model     hnd   {};
      overlay::model  olay  {};

      auto [map, textures] = mapper::load(sires::real_path_name(map_name));

      faces.load_textures(textures);

      lgm.load_map(&map);
      faces.load_map(map); 
      blt.load_map(&map);
      dr.load_map(&map);
      psh.load_map(&map);
      camera::load_map(&map);

      lgm.activate();

      input::on_button_down(input::buttons::ATTACK, hand::attack);
      input::on_button_down(input::buttons::USE, [&] {
        process_use(&dr, &psh);
      });

      sitime::stopwatch time {};
      bool copied = false;
      ots_loop(dq, sw, [&](auto cb) {
        // TODO: add a frame time limit or time interpolation
        bool moved = camera::update(map, time.millis());
        process_collisions(cb, blt);
        // TODO: squish
        psh.tick(time.millis());
        dr.tick(time.millis());
        hnd.tick(time.millis(), moved);
        time = {};

        if (!copied) {
          faces.setup_copy(cb);
          blt.setup_copy(cb);
          dr.setup_copy(cb);
          psh.setup_copy(cb);
          copied = true;
        }
        lgm.run(cb);
        dr.copy_models(cb);
        psh.copy_models(cb);

        voo::cmd_render_pass rp {{
          .command_buffer = cb,
          .render_pass = dq.render_pass(),
          .framebuffer = sw.framebuffer(),
          .extent = sw.extent(),
        }};
        vee::cmd_set_viewport(cb, sw.extent());
        vee::cmd_set_scissor(cb, sw.extent());
        faces.draw(cb);
        dr.draw(cb);
        psh.draw(cb);
        blt.draw(cb);

        hnd.run(cb);
        olay.run(cb, g_olay);
      });
    });
  } catch (const mapper::error & e) {
    silog::die("%s", (*e.msg).cstr().begin());
  }
} t;
