#include "app.h"
#include "engine.h"

eclipse::App* CreateApp();

#if defined(ECLIPSE_PLATFORM_WINDOWS)
int SDL_main(int, char**) {
#else
int main() {
#endif
  eclipse::App* app = CreateApp();
  eclipse::Engine::Instance().Run(app);
  delete app;
  return 0;
}
