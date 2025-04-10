#pragma leco tool

import jojo;
import jute;
import print;

static void (*g_liner)(jute::view);
static unsigned g_line_number;
static jute::view g_filename;

static void error(jute::view msg, auto arg) {
  die(g_filename, ":", g_line_number, ": ", msg, ": [", arg, "]");
}

static void check_version(jute::view arg) {
  arg = arg.trim();
  if (arg != "0") error("invalid version", arg);
}

static void take_command(jute::view line) {
  if (line == "") return;
  
  auto [cmd, args] = line.split(' ');
  if (cmd == "version") return check_version(args);

  error("unknown command", cmd);
}

int main() {
  g_filename = "example.map";
  g_liner = take_command;
  g_line_number = 1;
  jojo::readlines(g_filename, [](auto line) {
    g_liner(line.trim());
    g_line_number++;
  });
}
