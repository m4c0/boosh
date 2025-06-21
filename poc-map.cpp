#pragma leco tool

import entdefs;
import jojo;
import jute;
import mapper;
import print;

constexpr const jute::view filename = "example.map";

int main() try {
  entdefs::add({ .name = "bullet" });
  entdefs::add({ .name = "player" });
  entdefs::add({ .name = "pushwall" });

  entdefs::add({
    .name = "door",
    .rotates = entdefs::rotation::FIXED,
  });
  entdefs::add({
    .name = "wall",
    .grounded = false,
  });

  auto _ = mapper::load(filename);
  putln("map seems fine");
} catch (const mapper::error & e) {
  err(filename, ":", e.line_number, ": ", e.msg);
  return 1;
} catch (...) {
  return 1;
}
