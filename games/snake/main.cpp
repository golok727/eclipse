#include "../../src/main.h"
#include "snake_app.h"

// Separate Snake executable entry point.
// The reusable engine startup in src/main.h calls this factory.
eclipse::App* CreateApp() {
  return new eclipse::games::snake::SnakeApp;
}
