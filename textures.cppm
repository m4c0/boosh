export module textures;
import jute;
import hai;
import hashley;
import v;

namespace textures {
  export constexpr const auto max_smps = 16;

  static auto samplers() {
    hai::array<vee::sampler::type> res { max_smps };
    for (auto & s : res) s = *v::g->linear_sampler;
    return res;
  }

  struct node {
    voo::bound_image img {};
    unsigned idx {};
  };

  class cache_t {
    hashley::fin<node> m_data { 107 };
    voo::single_dset m_dset {
      vee::dsl_fragment_samplers(samplers()),
      vee::combined_image_sampler(max_smps),
    };
    unsigned m_next {};

  public:
    cache_t() {
      v::g->uber_set = m_dset.descriptor_set();
    }

    [[nodiscard]] constexpr auto & operator[](jute::view name) {
      return m_data[name];
    }

    unsigned get(jute::view name, hai::fn<void, vee::image_view::type> callback) {
      auto & i = m_data[name];
      if (*i.img.iv) {
        callback(*i.img.iv);
        return i.idx;
      }

      i.idx = m_next++;
      voo::load_image(name, v::g->pd, v::g->dq->queue(), &i.img, [this, &i, cb=traits::move(callback)] mutable {
        vee::update_descriptor_set(m_dset.descriptor_set(), 0, i.idx, *i.img.iv);
        cb(*i.img.iv);
      });
      return i.idx;
    }

    void dispose() {
      m_data = decltype(m_data) { 107 };
      m_next = 0;
      m_dset = {};
    }
  };
  auto & cache() {
    static cache_t i {};
    return i;
  }

  export void dispose() { cache().dispose(); }

  export unsigned get(jute::view name) { return cache().get(name, [](auto) {}); }

  export void get(jute::view name, hai::fn<void, vee::image_view::type> callback) {
    cache().get(name, traits::move(callback));
  }
}
