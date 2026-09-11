#include "app.h"
#include "engine.h"

eclipse::App* CreateApp();
int main(){
  eclipse::App* app = CreateApp();
  eclipse::Engine::Instance().Run(app);
  delete app;
  return 0;  
}
