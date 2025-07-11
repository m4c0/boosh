export module ppl_with_txt;
import shaders;
import textures;
import v;

namespace ppl_with_txt {
  auto merge(vee::gr_pipeline_params into, const vee::gr_pipeline_params & from) {
    into.pipeline_layout = from.pipeline_layout;
    into.render_pass     = from.render_pass;
    into.shaders         = from.shaders;
    return into;
  }

  export void cmd_bind(vee::command_buffer cb, v::grpl * ppl) {
    vee::cmd_bind_descriptor_set(cb, *ppl->layout, 0, v::g->lightmap.descriptor_set());
    vee::cmd_bind_descriptor_set(cb, *ppl->layout, 1, v::g->uber_set.descriptor_set());
    vee::cmd_bind_gr_pipeline(cb, *ppl->pipeline);
  }

  export template<typename PC>
  v::grpl create(jute::view shader, const vee::gr_pipeline_params & p) {
    v::grpl res {};
    res.layout = vee::create_pipeline_layout({
      .descriptor_set_layouts {{
        v::g->lightmap.descriptor_set_layout(),
        v::g->uber_set.descriptor_set_layout(),
      }},
      .push_constant_ranges {{ vee::vertex_push_constant_range<PC>() }},
    });
    res.pipeline = vee::create_graphics_pipeline(merge(p, {
      .pipeline_layout = *res.layout,
      .render_pass = *v::g->rp,
      .shaders {
        shaders::get(*(jute::heap(shader) + ".vert.spv")).pipeline_vert_stage("main", vee::specialisation_info<float>(v::g->dq->aspect_of())),
        shaders::get(*(jute::heap(shader) + ".frag.spv")).pipeline_frag_stage("main", vee::specialisation_info<unsigned>(99, 1)),
      },
    }));
    return res;
  }
};
