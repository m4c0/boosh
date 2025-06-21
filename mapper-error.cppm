export module mapper:error;
import hai;
import jute;

namespace mapper {
  export struct error {
    hai::cstr msg;
    unsigned line_number;

    error(jute::heap h) : msg { (*h).cstr() } {}
  };
}
