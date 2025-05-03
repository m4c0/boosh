#pragma leco add_resource "hand.png"
#pragma leco add_shader "hand.vert"
#pragma leco add_shader "hand.frag"
export module hand;
import v;

namespace hand {
  export class model {
    struct upc {
      dotz::vec2 pos { 0.2f };
      dotz::vec2 size { 0.8f };
    } m_pc {};

    v::x<upc> m_x {};
    voo::one_quad_render m_oqr;
    voo::h2l_image m_txt;

  public:
    explicit model(const voo::device_and_queue & dq, auto * aspect_k)
      : m_x {}
      , m_oqr { "hand", &dq, *m_x.pl }
      , m_txt { voo::load_image_file("hand.png", dq.physical_device()) }
    {
      m_x.update_descriptor_set(m_txt.iv());
    }

    void setup_copy(vee::command_buffer cb) {
      m_txt.setup_copy(cb);
    }

    void run(vee::command_buffer cb) {
      vee::cmd_bind_descriptor_set(cb, *m_x.pl, 0, m_x.ds.descriptor_set());
      vee::cmd_push_vertex_constants(cb, *m_x.pl, &m_pc);
      m_oqr.run(cb);
    }
  };
}
