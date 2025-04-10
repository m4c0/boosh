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
}
