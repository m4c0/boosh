export module v;
export import dotz;
export import jute;
export import voo;

namespace v {
  export template<typename PC> struct x {
    vee::sampler m_smp = vee::create_sampler(vee::linear_sampler);
    voo::single_frag_dset m_ds { 1 };
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(
      m_ds.descriptor_set_layout(),
      vee::vertex_push_constant_range<PC>()
    );
    voo::h2l_image m_txt;

  public:
    x(const voo::device_and_queue * dq, const char * txt) 
      : m_txt { voo::load_image_file(txt, dq->physical_device()) }
    {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, m_txt.iv(), *m_smp);
    }

    void setup_copy(vee::command_buffer cb) {
      m_txt.setup_copy(cb);
    }
  };
}
