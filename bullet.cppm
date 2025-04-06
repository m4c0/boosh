#pragma leco add_shader "bullet.frag"
#pragma leco add_shader "bullet.vert"
#pragma leco add_resource "bullet.obj"
#pragma leco add_resource "bullet.uv.png"
export module bullet;
import dotz;
import faces;
import hai;
import traits;
import voo;
import wavefront;

namespace bullet {
  hai::varray<dotz::vec3> list { 128 };
  export void add(dotz::vec3 p) { list.push_back(p); }
  export void clear() { list.truncate(0); }

  export bool take(dotz::vec3 p) {
    for (auto i = 0; i < list.size(); i++) {
      auto b = list[i];
      if (dotz::length(p - b) > 0.5) continue;

      list[i] = list.pop_back();
      return true;
    }
    return false;
  }

  export class model {
    struct upc {
      dotz::vec3 cam;
      float angle;
    };

    using mdl = faces::mdl;
    using vtx = wavefront::vtx;

    static constexpr const auto max_models = 128;

    vee::sampler m_smp = vee::create_sampler(vee::linear_sampler);
    voo::single_frag_dset m_ds { 1 };
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(m_ds.descriptor_set_layout(), vee::vertex_push_constant_range<upc>());
    vee::gr_pipeline m_gp;
    voo::h2l_buffer m_buf;
    voo::h2l_buffer m_mdl;
    voo::h2l_image m_txt;
    unsigned m_vcount;
    unsigned m_icount;
  public:
    explicit model(voo::device_and_queue & dq)
      : m_gp { vee::create_graphics_pipeline({
        .pipeline_layout = *m_pl,
        .render_pass = dq.render_pass(),
        .shaders {
          voo::shader("bullet.vert.spv").pipeline_vert_stage(),
          voo::shader("bullet.frag.spv").pipeline_frag_stage(),
        },
        .bindings {
          vee::vertex_input_bind_per_instance(sizeof(mdl)),
          vee::vertex_input_bind(sizeof(vtx)),
        },
        .attributes {
          vee::vertex_attribute_vec3(0, traits::offset_of(&mdl::pos)),
          vee::vertex_attribute_vec3(1, traits::offset_of(&vtx::pos)),
          vee::vertex_attribute_vec2(1, traits::offset_of(&vtx::txt)),
          vee::vertex_attribute_vec3(1, traits::offset_of(&vtx::nrm)),
        },
      }) }
      , m_mdl { dq.physical_device(), max_models * sizeof(mdl) }
      , m_txt { voo::load_image_file("bullet.uv.png", dq.physical_device()) }
    {
      auto [buf, count] = wavefront::load_model(dq.physical_device(), "bullet.obj");
      m_buf = traits::move(buf);
      m_vcount = count;

      vee::update_descriptor_set(m_ds.descriptor_set(), 0, m_txt.iv(), *m_smp);
    }

    void setup_copy(vee::command_buffer cb) {
      {
        voo::memiter<mdl> m { m_mdl.host_memory(), &m_icount };
        for (auto b : list) {
          m += { .pos = b };
        }
      }
      m_buf.setup_copy(cb);
      m_mdl.setup_copy(cb);
      m_txt.setup_copy(cb);
    }

    void draw(vee::command_buffer cb, dotz::vec3 cam, float angle) {
      upc pc { cam, angle };
      vee::cmd_bind_vertex_buffers(cb, 0, m_mdl.local_buffer());
      vee::cmd_bind_vertex_buffers(cb, 1, m_buf.local_buffer());
      vee::cmd_push_vertex_constants(cb, *m_pl, &pc);
      vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_ds.descriptor_set());
      vee::cmd_bind_gr_pipeline(cb, *m_gp);
      vee::cmd_draw(cb, m_vcount, m_icount);
    }
  };
}
