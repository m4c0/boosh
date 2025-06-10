export module textures;
import jute;
import hai;
import hashley;
import v;

namespace textures {
  struct node {
    voo::bound_image img {};
    unsigned idx {};
  };

  class cache_t {
    hashley::fin<node> m_data { 107 };
    unsigned m_next {};

  public:
    [[nodiscard]] constexpr auto & operator[](jute::view name) {
      return m_data[name];
    }

    unsigned get(jute::view name) {
      auto & i = m_data[name];
      if (*i.img.iv)  return i.idx;

      i.idx = m_next++;
      voo::load_image(name, v::g->pd, v::g->dq->queue(), &i.img, [&i] {
        vee::update_descriptor_set(v::g->uber_set.descriptor_set(), 0, i.idx, *i.img.iv);
      });
      return i.idx;
    }

    void dispose() {
      m_data = decltype(m_data) { 107 };
      m_next = 0;
    }
  };
  auto & cache() {
    static cache_t i {};
    return i;
  }

  export void dispose() { cache().dispose(); }

  export unsigned get(jute::view name) { return cache().get(name); }
}
