export module input;
import casein;

namespace input {
  export enum class axis { WALK, STRAFE, TURN };
  export float state(axis a);

  export void setup();
}

module :private;

namespace {
  enum keys {
    STRAFE_LEFT,
    STRAFE_RIGHT,
    FORWARD,
    BACKWARD,
    MAX_KEYS,
  };
}

static bool g_state[MAX_KEYS] {};

static void setup_btn(casein::keys k, keys i) {
  using namespace casein;
  handle(KEY_DOWN, k, [i] { g_state[i] = true;  });
  handle(KEY_UP,   k, [i] { g_state[i] = false; });
}

static int axis_state(keys n, keys p) {
  if (g_state[n] && !g_state[p]) return -1;
  if (!g_state[n] && g_state[p]) return 1;
  return 0;
}

float input::state(axis a) {
  switch (a) {
    case axis::WALK:   return axis_state(BACKWARD, FORWARD);
    case axis::STRAFE: return axis_state(STRAFE_LEFT, STRAFE_RIGHT);
    case axis::TURN:   return casein::mouse_rel.x;
  }
}

void input::setup() {
  using namespace casein;
  setup_btn(K_A, STRAFE_LEFT);
  setup_btn(K_D, STRAFE_RIGHT);
  setup_btn(K_W, FORWARD);
  setup_btn(K_S, BACKWARD);

  handle(MOUSE_MOVE, [] {
    mouse_pos = window_size / 2.0;
    interrupt(IRQ_MOUSE_POS);
  });
}
