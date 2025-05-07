export module input;
import casein;
import hai;

namespace input {
  export enum class axis { WALK, STRAFE, TURN };
  export enum class buttons { ATTACK, USE };
  export float state(axis a);
  export bool state(buttons b);

  export void on_button_down(buttons b, hai::fn<void> fn);

  export void setup();
}

module :private;

namespace {
  enum keys {
    ATTACK,
    USE,
    STRAFE_LEFT,
    STRAFE_RIGHT,
    FORWARD,
    BACKWARD,
    MAX_KEYS,
  };
}

static hai::fn<void> g_btn_down_cb[MAX_KEYS] {};
static bool g_state[MAX_KEYS] {};
static float g_mouse_rel_x {};

static void setup_btn(casein::keys k, keys i) {
  using namespace casein;
  handle(KEY_DOWN, k, [i] {
    auto old = g_state[i];
    g_state[i] = true;
    if (!old && g_btn_down_cb[i]) g_btn_down_cb[i]();
  });
  handle(KEY_UP,   k, [i] { g_state[i] = false; });
}
static void setup_btn(casein::mouse_buttons m, keys i) {
  using namespace casein;
  handle(MOUSE_DOWN, m, [i] {
    auto old = g_state[i];
    g_state[i] = true;
    if (!old && g_btn_down_cb[i]) g_btn_down_cb[i]();
  });
  handle(MOUSE_UP,   m, [i] { g_state[i] = false; });
}

static int axis_state(keys n, keys p) {
  if (g_state[n] && !g_state[p]) return -1;
  if (!g_state[n] && g_state[p]) return 1;
  return 0;
}

// TODO: apply smoothing when "speeding up"
static float mouse_state() {
  auto x = g_mouse_rel_x;
  g_mouse_rel_x *= 0.3;
  return x;
}

float input::state(axis a) {
  switch (a) {
    case axis::WALK:   return axis_state(BACKWARD, FORWARD);
    case axis::STRAFE: return axis_state(STRAFE_LEFT, STRAFE_RIGHT);
    case axis::TURN:   return mouse_state();
  }
}
bool input::state(buttons b) {
  switch (b) {
    case buttons::ATTACK: return g_state[ATTACK];
    case buttons::USE:    return g_state[USE];
  }
}

void input::on_button_down(buttons b, hai::fn<void> fn) {
  switch (b) {
    case buttons::ATTACK: g_btn_down_cb[ATTACK] = fn; break;
    case buttons::USE:    g_btn_down_cb[USE]    = fn; break;
  }
}

void input::setup() {
  using namespace casein;
  setup_btn(K_A, STRAFE_LEFT);
  setup_btn(K_D, STRAFE_RIGHT);
  setup_btn(K_W, FORWARD);
  setup_btn(K_S, BACKWARD);

  setup_btn(K_SPACE, USE);
  setup_btn(M_LEFT, ATTACK);

  handle(MOUSE_MOVE, [] {
    mouse_pos = window_size / 2.0;
    interrupt(IRQ_MOUSE_POS);
  });
  handle(MOUSE_MOVE_REL, [] {
    g_mouse_rel_x = mouse_rel.x * 0.5;
  });
  cursor_visible = false;
  interrupt(IRQ_CURSOR);
}
