export module textures;
import jute;
import hai;
import hashley;
import v;

namespace textures {
  auto & cache() {
    static hashley::fin<voo::bound_image> i { 107 };
    return i;
  }

  export void reset() {
    cache() = hashley::fin<voo::bound_image> { 107 };
  }
  export void get(jute::view name, hai::fn<void, vee::image_view::type> callback) {
    auto & i = cache()[name];
    if (*i.iv) {
      callback(*i.iv);
    } else {
      voo::load_image(name, v::g->pd, v::g->dq->queue(), &i, [&i,cb=traits::move(callback)]() mutable {
        cb(*i.iv);
      });
    }
  }
}
