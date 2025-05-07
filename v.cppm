export module v;
export import dotz;
export import hai;
export import jute;
export import voo;
import wagen;

namespace v {
  export template<typename PC> class simple_pipeline {
    voo::single_cb m_cb;
    voo::queue * m_q;

    vee::sampler m_smp = vee::create_sampler(vee::linear_sampler);
    voo::single_frag_dset m_ds { 1 };
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(
      m_ds.descriptor_set_layout(),
      vee::vertex_push_constant_range<PC>()
    );
    voo::h2l_image m_txt;
    vee::gr_pipeline m_pipeline;

    static auto merge(vee::gr_pipeline_params into, const vee::gr_pipeline_params & from) {
      into.pipeline_layout = from.pipeline_layout;
      into.render_pass     = from.render_pass;
      into.shaders         = from.shaders;
      return into;
    }

  public:
    simple_pipeline(voo::device_and_queue * dq, const char * txt, jute::view shader, const vee::gr_pipeline_params & p) 
      : m_cb { dq->queue_family() }
      , m_q { dq->queue() }
      , m_txt { voo::load_image_file(txt, dq->physical_device()) }
      , m_pipeline {
        vee::create_graphics_pipeline(merge(p, {
          .pipeline_layout = *m_pl,
          .render_pass = dq->render_pass(),
          .shaders {
            voo::shader(*(jute::heap(shader) + ".vert.spv")).pipeline_vert_stage("main", vee::specialisation_info<float>(dq->aspect_of())),
            voo::shader(*(jute::heap(shader) + ".frag.spv")).pipeline_frag_stage(),
          },
        }))
      }
    {
      vee::update_descriptor_set(m_ds.descriptor_set(), 0, m_txt.iv(), *m_smp);

      voo::cmd_buf_one_time_submit::build(m_cb.cb(), [this](auto cb) {
        m_txt.setup_copy(cb);
      });
      dq->queue()->queue_submit({ .command_buffer = m_cb.cb() });
    }

    void copy_image(const voo::host_buffer_for_image & img) {
      voo::cmd_buf_one_time_submit::build(m_cb.cb(), [&](auto cb) {
        img.setup_copy(cb, m_txt.image());
      });
      m_q->queue_submit({ .command_buffer = m_cb.cb() });
    }

    void cmd_bind(vee::command_buffer cb, const PC & pc) {
      vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_ds.descriptor_set());
      vee::cmd_push_vertex_constants(cb, *m_pl, &pc);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
    }
  };
}
