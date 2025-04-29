#pragma leco add_resource "hand.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import voo;

namespace hand {
  export class model {
    vee::sampler m_smp = vee::create_sampler(vee::linear_sampler);
    voo::single_dset m_ds {
      vee::dsl_fragment_sampler(),
      vee::combined_image_sampler()
    };
    vee::pipeline_layout m_pl = vee::create_pipeline_layout({
      m_ds.descriptor_set_layout()
    });
    voo::one_quad_render m_oqr;
    voo::h2l_image m_txt;

  public:
    explicit model(const voo::device_and_queue & dq)
      : m_oqr { "hand", &dq, *m_pl }
      , m_txt { voo::load_image_file("hand.png", dq.physical_device()) }
    {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, m_txt.iv(), *m_smp);
    }

    void setup_copy(vee::command_buffer cb) {
      m_txt.setup_copy(cb);
    }

    void run(vee::command_buffer cb) {
      vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_ds.descriptor_set());
      m_oqr.run(cb);
    }
  };
}
