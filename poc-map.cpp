#pragma leco tool

import jojo;
import jute;
import mapper;
import print;

constexpr const jute::view filename = "example.map";

int main() try {
  mapper::loader { filename };

  putln("map seems fine");
} catch (const mapper::loader::error & e) {
  err(filename, ":", e.line_number, ": ", e.msg);
  return 1;
} catch (...) {
  return 1;
}
