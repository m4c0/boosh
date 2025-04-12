#pragma leco tool

import jojo;
import jute;
import mapper;
import print;

int main() {
  jute::view filename = "example.map";

  try {
    mapper::loader { filename };
  } catch (const mapper::loader::error & e) {
    die(filename, ":", e.line_number, ": ", e.msg);
  }

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
}
