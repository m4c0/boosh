export module shaders;
import jute;
import voo;

namespace shaders {
  export auto get(jute::view name) {
    return voo::shader { name };
  }
}
