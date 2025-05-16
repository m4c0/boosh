export module v;
export import dotz;
export import hai;
export import jute;
export import voo;
import wagen;

namespace v {
  export struct globals {
    vee::physical_device pd;

    vee::sampler linear_sampler = vee::create_sampler(vee::linear_sampler);
    vee::sampler nearest_sampler = vee::create_sampler(vee::nearest_sampler);
  };
  export globals * g;

  export template<typename PC> class ppl_with_txt {
    voo::single_cb m_cb {};

    vee::descriptor_set_layout m_dsl = vee::create_descriptor_set_layout({
      vee::dsl_fragment_sampler(),
      vee::dsl_fragment_sampler(),
    });
    vee::descriptor_pool m_dpool = vee::create_descriptor_pool(2, { vee::combined_image_sampler(2) });
    vee::descriptor_set m_ds = vee::allocate_descriptor_set(*m_dpool, *m_dsl);;
    vee::pipeline_layout m_pl = vee::create_pipeline_layout(
      *m_dsl,
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
    ppl_with_txt(voo::device_and_queue * dq, vee::image_view::type lgm_iv, const char * txt, jute::view shader, const vee::gr_pipeline_params & p) 
      : m_txt { voo::load_image_file(txt, v::g->pd) }
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
      vee::update_descriptor_set(m_ds, 0, m_txt.iv(), *v::g->linear_sampler);
      vee::update_descriptor_set(m_ds, 1, lgm_iv, *v::g->linear_sampler);
      copy_image();
    }

    void copy_image() {
      voo::cmd_buf_one_time_submit::build(m_cb.cb(), [&](auto cb) {
        m_txt.setup_copy(cb);
      });
      voo::queue::instance()->queue_submit({ .command_buffer = m_cb.cb() });
    }
    void copy_image(const voo::host_buffer_for_image & img) {
      voo::cmd_buf_one_time_submit::build(m_cb.cb(), [&](auto cb) {
        img.setup_copy(cb, m_txt.image());
      });
      voo::queue::instance()->queue_submit({ .command_buffer = m_cb.cb() });
    }

    void cmd_bind(vee::command_buffer cb, const PC & pc) {
      vee::cmd_bind_descriptor_set(cb, *m_pl, 0, m_ds);
      vee::cmd_push_vertex_constants(cb, *m_pl, &pc);
      vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
    }
  };
}
