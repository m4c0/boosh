#pragma leco tool

import jojo;
import jute;
import print;

static void (*g_liner)(jute::view);
static unsigned g_line_number;
static jute::view g_filename;

static void error(jute::view line) {
  die(g_filename, ":", g_line_number, ": invalid line: [", line, "]");
}

static void take_command(jute::view line) {
  if (line == "") return;
  putln(line);
  g_liner = error;
}

int main() {
  g_filename = "example.map";
  g_liner = take_command;
  g_line_number = 1;
  jojo::readlines(g_filename, [](auto line) {
    g_liner(line);
    g_line_number++;
  });
}
