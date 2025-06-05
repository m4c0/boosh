#pragma leco app
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

dotz::vec4 g_olay {};

static bool update_camera(const mapper::tilemap & map, float ms) {
  float da = -input::state(input::axis::TURN) * turn_speed * ms / 1000.0;
  v::g->camera.angle = dotz::mod(360 + v::g->camera.angle + da, 360);

  dotz::vec2 dr {
    input::state(input::axis::STRAFE),
    input::state(input::axis::WALK)
  };
  if (dotz::sq_length(dr) == 0) return false;

  float a = dotz::radians(v::g->camera.angle);
  float c = dotz::cos(a);
  float s = dotz::sin(a);
  auto d = dotz::normalise(dr);

  const auto walk = [&](auto dx, auto dy) {
    auto cam = v::g->camera.cam;
    cam.x -= dx * walk_speed * ms / 1000.0;
    cam.z += dy * walk_speed * ms / 1000.0;

    dotz::vec2 cxz { cam.x, cam.z };
    if (collision::bodies().closest(cxz, player_radius).owner) {
      return false;
    }

    v::g->camera.cam = cam;
    return true;
  };

  auto dx = d.x * c - d.y * s;
  auto dy = d.x * s + d.y * c;
  return walk(dx, dy) || walk(dx, 0) || walk(0, dy);
}

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

static void process_use(door::model * dr) {
  auto cam = v::g->camera.cam.xz();
  auto angle = dotz::radians(v::g->camera.angle);
  auto c = collision::entities().hitscan(cam, angle, max_use_dist);
  switch (c.item.owner) {
    case door::clid:
      dr->open(c.item.id);
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
      v::globals vg { &dq };
      v::g = &vg;

      v::check_max_dset(dset_smps);

      lightmap::pipeline lgm {};
      v::g->lightmap = lgm.output_iv();

      faces::model   faces { textures };
      bullet::model  blt   {};
      door::model    dr    {};
      hand::model    hnd   {};
      overlay::model olay  {};

      auto wcount = 0;
      map.for_each([&](auto x, auto y, auto & d) {
        // TODO: fix inverted camera Y
        switch (d.entity) {
          case mapper::entities::PLAYER:
            v::g->camera.cam = { x + 0.5f, -0.5f, y + 0.5f };
            break;
          case mapper::entities::PUSHWALL:
            pushwall::add({ x, y }, wcount);
            break;
          case mapper::entities::WALL:
            collision::bodies().add_aabb({ x, y }, { x + 1, y + 1 }, 'wall', 1);
            break;
          case mapper::entities::BULLET:
          case mapper::entities::DOOR:
          case mapper::entities::NONE:
            break;
        }
        if (d.wall) wcount++;
      });

      lgm.load_map(&map);
      faces.load_map(map); 
      blt.load_map(&map);
      dr.load_map(&map);

      input::on_button_down(input::buttons::ATTACK, hand::attack);
      input::on_button_down(input::buttons::USE, [&] {
        process_use(&dr);
      });

      sitime::stopwatch time {};
      bool copied = false;
      ots_loop(dq, sw, [&](auto cb) {
        // TODO: add a frame time limit or time interpolation
        bool moved = update_camera(map, time.millis());
        process_collisions(cb, blt);
        // TODO: squish
        pushwall::tick(faces, time.millis());
        dr.tick(time.millis());
        hnd.tick(time.millis(), moved);
        time = {};

        if (!copied) {
          blt.setup_copy(cb);
          dr.setup_copy(cb);
          copied = true;
        }
        faces.setup_copy(cb);
        lgm.run(cb);
        dr.copy_models(cb);

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
        blt.draw(cb);

        hnd.run(cb);
        olay.run(cb, g_olay);
      });
    });
  } catch (const mapper::error & e) {
    silog::die("%s", (*e.msg).cstr().begin());
  }
} t;
