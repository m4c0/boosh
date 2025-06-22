#pragma leco tool

import bullet;
import door;
import mob;
import pushwall;

import entdefs;
import jute;
import mapper;
import print;

constexpr const jute::view filename = "example.map";

int main() try {
  entdefs::add({ .name = bullet::item::entity });
  entdefs::add({ .name = mob::item::entity });
  entdefs::add({ .name = pushwall::item::entity });
  entdefs::add({
    .name = door::item::entity,
    .rotates = entdefs::rotation::FIXED,
  });

  entdefs::add({ .name = "player" });
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
