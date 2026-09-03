#include "engine.h"
#include "log.h"
#include <SDL2/SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_version.h>
#include "glad/glad.h"
#include "../input/mouse.h"
#include "../input/keyboard.h"
#include "app.h"
namespace eclipse {
Engine::Engine():mIsInitialized(false),mIsRunning(false){}
Engine::~Engine() {}

Engine& Engine::Instance(){
  if(!mInstance){
    mInstance = new Engine;
  }
  return* mInstance;
}

Engine* Engine::mInstance = nullptr;


void Engine::Run(App* app){


  mLogManager.Initialize();
ECLIPSE_ASSERT(!mApp, "Trying to re run app");
  if(mApp){
    return;
  }
  mApp = app;
  
  if(Initialize()){
    while(mIsRunning){

    
      Update();
      Render();    

    }
    Shutdown();
  }
}

void Engine::Update(){
    mWindow.PollEvents();
    mApp ->Update();
  }

void Engine::Render(){
      mWindow.BeginRender();
      mApp ->Render();
      mWindow.EndRender();
  
}

void Engine::Quit(){
  mIsRunning = false;
}

bool Engine::Initialize() {
  bool ret = false;
  GetInfo();
  ECLIPSE_ASSERT(!mIsInitialized, "Re Initialzing engine");
  if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
    ECLIPSE_ERROR("Error initalzing SDL {}", SDL_GetError());
    ret = false;
  } else {
    SDL_version version;
    SDL_VERSION(&version);
    ECLIPSE_INFO("SDL {}.{}.{}", (int)version.major, (int)version.minor,
                  (int)version.patch);
    core::WindowProperties props = mApp ->GetWindowProperties(); 
    if(mWindow.Create(props)){
      ret = true;
      mIsInitialized = true;
      mIsRunning = true;
      mApp->Initialize();
      mRenderManager.Initialize();
      input::mouse::Initialize();
      input::keyboard::Initialize();
    }
  }
  return ret;
}
void Engine::Shutdown() {
  mLogManager.Shutdown();
  mWindow.Shutdown();
  mApp ->Shutdown();
  mIsInitialized = false;
  mIsRunning = false;
  SDL_Quit();
}
void Engine::GetInfo() {
#ifdef ECLIPSE_CONFIG_DEBUG
  ECLIPSE_DEBUG("Configuration: DEBUG");
#endif
#ifdef ECLIPSE_CONFIG_RELEASE
  ECLIPSE_DEBUG("Configuration: RELEASE");
#endif
#ifdef ECLIPSE_PLATFORM_MAC
  ECLIPSE_WARN("Platform: MAC");
#endif
#ifdef ECLIPSE_PLATFORM_WINDOWS
  ECLIPSE_WARN("Platform: WINDOWS");
#endif
#ifdef ECLIPSE_PLATFORM_LINUX
  ECLIPSE_WARN("Platform: LINUX");
#endif
}
} // namespace eclipse
