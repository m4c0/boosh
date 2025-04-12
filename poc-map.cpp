#pragma leco tool

import jojo;
import jute;
import mapper;
import print;

int main() {
  try {
    mapper::loader { "example.map" };
  } catch (const mapper::loader::error & e) {
    die(e.filename, ":", e.line_number, ": ", e.msg, ": [", e.arg, "]");
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
