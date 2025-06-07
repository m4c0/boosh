export module shaders;
import jute;
import hai;
import hashley;
import voo;

namespace shaders {
  auto & cache() {
    static hashley::fin<voo::shader> i { 107 };
    return i;
  }

  export void reset() {
    cache() = hashley::fin<voo::shader> { 107 };
  }
  export auto & get(jute::view name) {
    auto & i = cache()[name];
    if (!i) i = voo::shader { name };
    return i;
  }
}
