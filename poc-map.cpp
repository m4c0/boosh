#pragma leco tool

import jojo;
import jute;
import mapper;
import print;

class loader : public mapper::loader {
  void error(jute::view msg, jute::view arg) override {
    die(filename(), ":", line_number(), ": ", msg, ": [", arg, "]");
  }
public:
  using mapper::loader::loader;
};

int main() {
  loader l { "example.map" };

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
