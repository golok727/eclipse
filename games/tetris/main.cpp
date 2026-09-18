#include "../../src/main.h"
#include "tetris_app.h"

// Separate Tetris executable entry point.
eclipse::App* CreateApp() {
  return new eclipse::games::tetris::TetrisApp;
}
