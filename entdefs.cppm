export module entdefs;
import jute;
import hai;
import silog;

namespace entdefs {
  export enum class rotation {
    NONE, FIXED /* 0 or 90 degrees */, FREELY
  };
  export struct t {
    jute::view name;
    rotation rotates = rotation::NONE;
    bool grounded = true; // requires floor/ceiling
  };

  export void add(t t);
  export const t * for_name(jute::view name);
}

module :private;

static hai::varray<entdefs::t> g_list { 128 };

void entdefs::add(entdefs::t t) {
  if (g_list.size() == g_list.capacity()) silog::die("too many entdefs");
  if (for_name(t.name)) silog::die("duplicated entdef: %s", t.name.cstr().begin());
  g_list.push_back(t);
}
const entdefs::t * entdefs::for_name(jute::view name) {
  for (auto & t : g_list) {
    if (t.name == name) return &t;
  }
  return nullptr;
}
