export module mapper:error;
import jute;

namespace mapper {
  export struct error {
    jute::heap msg;
    unsigned line_number;
  };
}
