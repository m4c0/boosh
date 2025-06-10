#pragma leco add_resource "door.obj"
#pragma leco add_resource "door.uv.png"
export module door;
import collision;
import dotz;
import hai;
import mapper;
import model;
import textures;
import voo;

namespace door {
  export constexpr const auto clid = 'door';

  constexpr const auto door_speed = 1.0f;
  constexpr const auto stop_y = 0.9f;
  constexpr const auto max = 128;

  struct item {
    dotz::vec3 pos;
    float rot;
    float movement;
  };

  export class model : public ::model::batch {
    hai::varray<item> m_list { max };

    void load(voo::memiter<mdl> & m) override {
      for (auto p : m_list) m += {
        .pos = p.pos,
        .rot = p.rot,
        .txt = textures::get("door.uv.png"),
      };
    }

  public:
    explicit model() : batch { "door.obj" } {}

    void open(unsigned id) {
      m_list[id].movement = 1;
    }

    void load_map(const mapper::tilemap * map) {
      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity != mapper::entities::DOOR) return; 

        dotz::vec3 p { x + 0.5f, 0.0f, y + 0.5f };
        dotz::vec2 aa = dotz::floor(p.xz());
        dotz::vec2 bb = aa + 1;
        if (d.rotate == 0) {
          aa.x += 0.4;
          bb.x = aa.x + 0.2;
        } else {
          aa.y += 0.4;
          bb.y = aa.y + 0.2;
        }
        collision::bodies().add_aabb(aa, bb, clid, m_list.size());
        collision::entities().add_aabb(aa, bb, clid, m_list.size());
        float r = dotz::radians(d.rotate);
        m_list.push_back(item { p, r, 0 });
      });
    }

    void tick(float ms) {
      auto m = memiter();
      for (auto i = 0; i < m_list.size(); i++) {
        auto & it = m_list[i];
        if (it.movement == 0) continue;

        float y = it.pos.y - door_speed * ms / 1000.0;
        if (y < -stop_y) {
          y = -stop_y;
          it.movement = 0;
          collision::bodies().remove(clid, i);
          collision::entities().remove(clid, i);
        }
        m[i].pos.y = it.pos.y = y;
      }
    }
  };
}
