#pragma leco tool

import jojo;
import jute;
import print;

static void (*g_liner)(jute::view);
static unsigned g_line_number;
static jute::view g_filename;

static char g_bullet_id;
static char g_hall_id;
static char g_player_id;
static char g_wall_id;

static void error(jute::view msg, auto arg) {
  die(g_filename, ":", g_line_number, ": ", msg, ": [", arg, "]");
}

static void check_version(jute::view arg) {
  if (arg != "0") error("invalid version", arg);
}

static void set_id(char * g, jute::view arg) {
  if (arg.size() != 1) error("invalid character id", arg);
  *g = arg[0];
}

static void take_command(jute::view line) {
  if (line == "") return;
  
  auto [cmd, args] = line.split(' ');
  args = args.trim();

  if (cmd == "version") return check_version(args);
  if (cmd == "bullet")  return set_id(&g_bullet_id, args);
  if (cmd == "hall")    return set_id(&g_hall_id,   args);
  if (cmd == "player")  return set_id(&g_player_id, args);
  if (cmd == "wall")    return set_id(&g_wall_id,   args);

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
