export module v;
export import dotz;
export import voo;

namespace v {
  export template<typename PC> struct x {
    vee::sampler m_smp = vee::create_sampler(vee::linear_sampler);
    voo::single_frag_dset m_ds { 1 };
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(
      m_ds.descriptor_set_layout(),
      vee::vertex_push_constant_range<PC>()
    );
    void update_descriptor_set(vee::image_view::type iv) {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, iv, *m_smp);
    }
  };
}
