export module ppl_with_txt;
import shaders;
import textures;
import v;

export template<typename PC> class ppl_with_txt {
  vee::pipeline_layout m_pl = vee::create_pipeline_layout({
    .descriptor_set_layouts {{
      v::g->lightmap.descriptor_set_layout(),
      v::g->uber_set.descriptor_set_layout(),
    }},
    .push_constant_ranges {{ vee::vertex_push_constant_range<PC>() }},
  });
  vee::gr_pipeline m_pipeline;

  static auto merge(vee::gr_pipeline_params into, const vee::gr_pipeline_params & from) {
    into.pipeline_layout = from.pipeline_layout;
    into.render_pass     = from.render_pass;
    into.shaders         = from.shaders;
    return into;
  }

public:
  ppl_with_txt(jute::view shader, const vee::gr_pipeline_params & p) 
    : m_pipeline {
      vee::create_graphics_pipeline(merge(p, {
        .pipeline_layout = *m_pl,
        .render_pass = v::g->dq->render_pass(),
        .shaders {
          shaders::get(*(jute::heap(shader) + ".vert.spv")).pipeline_vert_stage("main", vee::specialisation_info<float>(v::g->dq->aspect_of())),
          shaders::get(*(jute::heap(shader) + ".frag.spv")).pipeline_frag_stage("main", vee::specialisation_info<unsigned>(99, 1)),
        },
      }))
    }
  {}

  void cmd_bind(vee::command_buffer cb, const PC & pc) {
    vee::cmd_bind_descriptor_set(cb, *m_pl, 0, v::g->lightmap.descriptor_set());
    vee::cmd_bind_descriptor_set(cb, *m_pl, 1, v::g->uber_set.descriptor_set());
    vee::cmd_push_vertex_constants(cb, *m_pl, &pc);
    vee::cmd_bind_gr_pipeline(cb, *m_pipeline);
  }
};
