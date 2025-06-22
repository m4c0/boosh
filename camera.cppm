export module camera;
import collision;
import input;
import mapper;
import v;

static constexpr const auto turn_speed = 180.0f;
static constexpr const auto walk_speed = 5.0f;

static constexpr const auto player_radius = 0.2f;

namespace camera {
  export void load_map(const mapper::tilemap * map) {
    map->for_each([&](auto x, auto y, auto & d) {
      // TODO: fix inverted camera Y
      if (!d.entity || d.entity->name != "player") return;
      v::g->camera.cam = { x + 0.5f, -0.5f, y + 0.5f };
    });
  }

  export bool update(float ms) {
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
}

