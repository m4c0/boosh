export module ppl_with_txt;
import v;

export template<typename PC> class ppl_with_txt {
  voo::single_cb m_cb {};

  voo::single_frag_dset m_dset { 1 };
  vee::pipeline_layout m_pl = vee::create_pipeline_layout({
    .descriptor_set_layouts {{
      v::g->lightmap.descriptor_set_layout(),
      m_dset.descriptor_set_layout(),
    }},
    .push_constant_ranges {{ vee::vertex_push_constant_range<PC>() }},
  });
  voo::h2l_image m_txt;
  vee::gr_pipeline m_pipeline;

  static auto merge(vee::gr_pipeline_params into, const vee::gr_pipeline_params & from) {
    into.pipeline_layout = from.pipeline_layout;
    into.render_pass     = from.render_pass;
    into.shaders         = from.shaders;
    return into;
  }

public:
  ppl_with_txt(const char * txt, jute::view shader, const vee::gr_pipeline_params & p) 
    : m_txt { voo::load_image_file(txt, v::g->pd) }
    , m_pipeline {
      vee::create_graphics_pipeline(merge(p, {
        .pipeline_layout = *m_pl,
        .render_pass = v::g->dq->render_pass(),
        .shaders {
          voo::shader(*(jute::heap(shader) + ".vert.spv")).pipeline_vert_stage("main", vee::specialisation_info<float>(v::g->dq->aspect_of())),
          voo::shader(*(jute::heap(shader) + ".frag.spv")).pipeline_frag_stage("main", vee::specialisation_info<unsigned>(99, 1)),
        },
      }))
    }
  {
    vee::update_descriptor_set(m_dset.descriptor_set(), 0, m_txt.iv(), *v::g->linear_sampler);
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
    vee::cmd_bind_descriptor_set(cb, *m_pl, 0, v::g->lightmap.descriptor_set());
    vee::cmd_bind_descriptor_set(cb, *m_pl, 1, m_dset.descriptor_set());
    vee::cmd_push_vertex_constants(cb, *m_pl, &pc);
    vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
  }
};
