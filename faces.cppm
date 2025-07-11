export module faces;
import collision;
import dotz;
import mapper;
import shaders;
import textures;
import traits;
import v;

namespace faces {
  struct vtx {
    dotz::vec3 pos;
    dotz::vec2 uv;
    dotz::vec3 nrm;
  };
  struct inst {
    dotz::vec3 pos;
    float rot;
    unsigned txt;
  };

  inline const auto bindings() {
    return decltype(vee::gr_pipeline_params::bindings) {
      vee::vertex_input_bind(sizeof(vtx)),
      vee::vertex_input_bind_per_instance(sizeof(inst)),
    };
  }
  inline const auto attributes() {
    return decltype(vee::gr_pipeline_params::attributes) {
      vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::pos)),
      vee::vertex_attribute_vec2(0, traits::offset_of(&vtx::uv)),
      vee::vertex_attribute_vec3(0, traits::offset_of(&vtx::nrm)),
      vee::vertex_attribute_vec4(1, traits::offset_of(&inst::pos)),
      vee::vertex_attribute_uint(1, traits::offset_of(&inst::txt)),
    };
  }

  class instanced {
    static constexpr const auto max_inst = 256 * 256;

    voo::h2l_buffer m_vtx;
    voo::h2l_buffer m_inst;
    unsigned m_v_count;
    unsigned m_i_count;

  protected:
    [[nodiscard]] auto map_vertex() {
      return voo::memiter<vtx> { m_vtx.host_memory() };
    }

  public:
    explicit instanced(unsigned vcount)
      : m_vtx { v::g->pd, static_cast<unsigned>(sizeof(vtx) * vcount) }
      , m_inst { v::g->pd, sizeof(inst) * max_inst }
      , m_v_count { vcount }
    {}

    [[nodiscard]] auto map() {
      return voo::memiter<inst> { m_inst.host_memory(), &m_i_count };
    }
    [[nodiscard]] auto remap() {
      return voo::memiter<inst> { m_inst.host_memory() };
    }

    void setup_copy(vee::command_buffer cb) {
      m_vtx.setup_copy(cb);
      m_inst.setup_copy(cb);
    }

    void draw(vee::command_buffer cb) {
      vee::cmd_bind_vertex_buffers(cb, 0, m_vtx.local_buffer());
      vee::cmd_bind_vertex_buffers(cb, 1, m_inst.local_buffer());
      vee::cmd_draw(cb, m_v_count, m_i_count);
    }
  };

  class ceiling : public instanced {
  public:
    explicit ceiling() : instanced { 6 } {
      auto m = map_vertex();

      m += vtx { .pos = { 0, 1, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 1, 1, 0 }, .uv = { 1, 0 } };
      m += vtx { .pos = { 1, 1, 1 }, .uv = { 1, 1 } };
    
      m += vtx { .pos = { 1, 1, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 0, 1, 1 }, .uv = { 0, 1 } };
      m += vtx { .pos = { 0, 1, 0 }, .uv = { 0, 0 } };
    }
  };

  class floor : public instanced {
  public:
    explicit floor() : instanced { 6 } {
      auto m = map_vertex();

      m += vtx { .pos = { 0, 0, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 1, 0, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 1, 0, 0 }, .uv = { 1, 0 } };
    
      m += vtx { .pos = { 1, 0, 1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { 0, 0, 0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { 0, 0, 1 }, .uv = { 0, 1 } };
    }
  };

  class wall : public instanced {
    static void x_wall(voo::memiter<vtx> & m, float x0, float x1, float y) {
      m += vtx { .pos = { x0, 0.f, y }, .uv = { 0, 1 } };
      m += vtx { .pos = { x1, 0.f, y }, .uv = { 1, 1 } };
      m += vtx { .pos = { x1, 1.f, y }, .uv = { 1, 0 } };
    
      m += vtx { .pos = { x1, 1.f, y }, .uv = { 1, 0 } };
      m += vtx { .pos = { x0, 1.f, y }, .uv = { 0, 0 } };
      m += vtx { .pos = { x0, 0.f, y }, .uv = { 0, 1 } };
    }
    static void y_wall(voo::memiter<vtx> & m, float x, float y0, float y1) {
      m += vtx { .pos = { x, 0.f, y0 }, .uv = { 0, 1 } };
      m += vtx { .pos = { x, 0.f, y1 }, .uv = { 1, 1 } };
      m += vtx { .pos = { x, 1.f, y1 }, .uv = { 1, 0 } };

      m += vtx { .pos = { x, 1.f, y1 }, .uv = { 1, 0 } };
      m += vtx { .pos = { x, 1.f, y0 }, .uv = { 0, 0 } };
      m += vtx { .pos = { x, 0.f, y0 }, .uv = { 0, 1 } };
    }

  public:
    explicit wall() : instanced { 24 } {
      auto m = map_vertex();
      x_wall(m, 0, 1, 1);
      x_wall(m, 1, 0, 0);
      y_wall(m, 0, 0, 1);
      y_wall(m, 1, 1, 0);
    }
  };

  export class model {
    faces::ceiling m_ceilings {};
    faces::floor   m_floors   {};
    faces::wall    m_walls    {};

    vee::descriptor_set_layout m_dsl;
    vee::pipeline_layout m_pl;
    vee::gr_pipeline m_gp;

  public:
    explicit model()
      : m_pl { vee::create_pipeline_layout({
        .descriptor_set_layouts {{
          v::g->lightmap.descriptor_set_layout(),
          v::g->uber_set.descriptor_set_layout(),
        }},
        .push_constant_ranges {{ vee::vertex_push_constant_range<v::camera>() }},
      }) }
      , m_gp { vee::create_graphics_pipeline({
        .pipeline_layout = *m_pl,
        .render_pass = *v::g->rp,
        .shaders {
          shaders::get("model.vert.spv").pipeline_vert_stage("main", vee::specialisation_info<float>(v::g->dq->aspect_of())),
          shaders::get("model.frag.spv").pipeline_frag_stage("main"),
        },
        .bindings   = bindings(),
        .attributes = attributes(),
      }) }
    {}

    void load_map(const mapper::tilemap * map) {
      auto c = m_ceilings.map();
      auto f = m_floors.map();
      auto w = m_walls.map();
      map->for_each([&](auto x, auto y, auto & d) {
        if (d.entity && d.entity->name == "wall") {
          collision::bodies().add_aabb({ x, y }, { x + 1, y + 1 }, 'wall', 1);
          w += { { x, 0, y }, 0, textures::get("Tiles040_1K-JPG_Color.jpg") };
        }

        if (d.floor.size())   f += { { x, 0, y }, 0, textures::get(*d.floor)   };
        if (d.ceiling.size()) c += { { x, 0, y }, 0, textures::get(*d.ceiling) };
      });
    }
    [[nodiscard]] auto remap_walls() { return m_walls.remap(); }

    void setup_copy(vee::command_buffer cb) {
      m_ceilings.setup_copy(cb);
      m_floors.setup_copy(cb);
      m_walls.setup_copy(cb);
    }

    void draw(vee::command_buffer cb) {
      vee::cmd_push_vertex_constants(cb, *m_pl, &v::g->camera);
      vee::cmd_bind_gr_pipeline(cb, *m_gp);
      vee::cmd_bind_descriptor_set(cb, *m_pl, 0, v::g->lightmap.descriptor_set());
      vee::cmd_bind_descriptor_set(cb, *m_pl, 1, v::g->uber_set.descriptor_set());

      m_ceilings.draw(cb);
      m_floors.draw(cb);
      m_walls.draw(cb);
    }
  };
}
