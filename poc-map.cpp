#pragma leco tool

import jojo;
import jute;
import print;

static void (*g_liner)(jute::view);

static void error(jute::view line) {
  die("invalid line: [", line, "]");
}

static void take_command(jute::view line) {
  putln(line);
  g_liner = error;
}

int main() {
  g_liner = take_command;
  jojo::readlines("example.map", [](auto line) { g_liner(line); });
}
