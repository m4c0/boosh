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
import entdefs;
import faces;
import hand;
import input;
import lightmap;
import mapper;
import mob;
import model;
import overlay;
import pushwall;
import shaders;
import silog;
import sires;
import sitime;
import textures;
import traits;
import v;
import vapp;

using namespace jute::literals;

static constexpr const jute::view map_name = "example.map";

static constexpr const auto player_radius = 0.2f;
static constexpr const auto max_use_dist = 1.0f;

class stuff {
  lightmap::pipeline m_lgm {};

  model::list<bullet::item>   m_blt   {};
  model::list<door::item>     m_dr    {};
  model::list<mob::item>      m_mob   {};
  model::list<pushwall::item> m_psh   {};

  faces::model    m_faces {};
  hand::model     m_hnd   {};
  overlay::model  m_olay  {};

  bool m_copied = false;

public:
  stuff() {
    model::create_pipeline();
  }

  void load_map(const mapper::tilemap * map) {
    m_lgm.load_map(map);
    m_faces.load_map(map);
    m_blt.load_map(map);
    m_mob.load_map(map);
    m_dr.load_map(map);
    m_psh.load_map(map);

    m_lgm.activate();
  }

  void use() {
    auto cam = v::g->camera.cam.xz();
    auto angle = dotz::radians(v::g->camera.angle);
    auto c = collision::entities().hitscan(cam, angle, max_use_dist);
    switch (c.item.owner) {
      case door::clid:
        m_dr.use(c.item.id);
        break;
      case pushwall::clid:
        m_psh.use(c.item.id);
        break;
    }
  }

  void tick(vee::command_buffer cb, float ms, bool moved) {
    auto cam = v::g->camera.cam;
    auto item = collision::entities().closest({ cam.x, cam.z }, player_radius);
    switch (item.owner) {
      case bullet::clid:
        m_blt.remove(item.id);
        m_olay.set(0.5f);
        break;
    }

    // TODO: squish
    m_psh.tick(ms);
    m_dr.tick(ms);
    m_hnd.tick(ms, moved);
    m_olay.tick(ms);

    if (!m_copied) {
      m_faces.setup_copy(cb);
      m_copied = true;
    }
    m_lgm.run(cb);
  }

  void draw(vee::command_buffer cb) {
    m_faces.draw(cb);

    model::bind(cb);
    m_dr.draw(cb);
    m_psh.draw(cb);
    m_blt.draw(cb);
    m_mob.draw(cb);

    m_hnd.run(cb);
    m_olay.run(cb);
  }
};

struct : public vapp {
  void run() try {
    entdefs::add({ .name = bullet::item::entity });
    entdefs::add({ .name = mob::item::entity });
    entdefs::add({ .name = pushwall::item::entity });
    entdefs::add({
      .name = door::item::entity,
      .rotates = entdefs::rotation::FIXED,
    });

    entdefs::add({ .name = "player" });
    entdefs::add({
      .name = "wall",
      .grounded = false,
    });

    input::setup();
    main_loop("poc-voo", [&](auto & dq) {
      v::globals vg { &dq };
      v::g = &vg;

      voo::offscreen::depth_buffer depth { v::g->pd, v::g->dq->extent_of() };
      voo::swapchain_and_stuff sw { dq, *v::g->rp, depth.image_view() };

      stuff s {};

      auto map = mapper::load(sires::real_path_name(map_name));

      s.load_map(&map);
      camera::load_map(&map);

      input::on_button_down(input::buttons::ATTACK, hand::attack);
      input::on_button_down(input::buttons::USE, [&] { s.use(); });

      sitime::stopwatch time {};
      ots_loop(dq, sw, [&] {
        auto cb = sw.command_buffer();

        // TODO: add a frame time limit or time interpolation
        bool moved = camera::update(time.millis());
        s.tick(cb, time.millis(), moved);
        time = {};

        voo::cmd_render_pass rp { vee::render_pass_begin {
          .command_buffer = cb,
          .render_pass = *v::g->rp,
          .framebuffer = sw.framebuffer(),
          .extent = sw.extent(),
        }};
        vee::cmd_set_viewport(cb, sw.extent());
        vee::cmd_set_scissor(cb, sw.extent());
        s.draw(cb);
      });

      shaders::dispose();
      textures::dispose();
    });
  } catch (const mapper::error & e) {
    silog::die("Error in map: %s", e.msg.begin());
  }
} t;
