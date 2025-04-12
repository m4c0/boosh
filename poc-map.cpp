#pragma leco tool

import jojo;
import jute;
import mapper;
import print;

constexpr const jute::view filename = "example.map";

int main() try {
  mapper::loader { filename };

  for (auto y = 0; y < 32; y++) {
    for (auto x = 0; x < 64; x++) {
      switch (mapper::tiles[y][x]) {
        case mapper::tile::empty: put(" "); break;
        case mapper::tile::hall:  put("."); break;
        case mapper::tile::wall:  put("X"); break;
      }
    }
    putln();
  }
} catch (const mapper::loader::error & e) {
  err(filename, ":", e.line_number, ": ", e.msg);
  return 1;
} catch (...) {
  return 1;
}
