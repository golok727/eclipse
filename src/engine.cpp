#include "engine.h"
#include "../src/log.h"
#include <SDL2/SDL.h>
#include <SDL_error.h>
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../input/actions.h"
#include <string>
namespace eclipse {
Engine::Engine():mIsInitialized(false),mIsRunning(false),mApp(nullptr){
  
}

Engine::~Engine() {
  if(mApp){
    Shutdown();
  }
}

void Engine::Run(App* app){
  mLogManager.Initialize();
  ECLIPSE_ASSERT(!mApp, "trying to reinitialize app ");
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
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_F5)) {
    ReloadScene();
    return;
  }
  if (input::ActionPressed(input::Action::Pause)) {
    const auto state = mGameStateManager.Current();
    mGameStateManager.Set(state == managers::GameState::Paused
                              ? managers::GameState::Playing
                              : managers::GameState::Paused);
  }
  if (mGameStateManager.Current() != managers::GameState::Playing) {
    return;
  }
  mClock.Tick();
  const float deltaTime = mClock.GetDeltaTime();
  mMovementSystem.Update(mWorld, deltaTime);
  mNpcSystem.Update(mWorld, deltaTime);
  mCollisionSystem.Update(mWorld);
  mAnimationSystem.Update(mWorld, deltaTime);
  mApp->Update(mWorld, deltaTime);
  mCameraSystem.Update(mWorld, mWindow.GetSize());
  mWorld.FlushDestroyed();
}

void Engine::ReloadScene() {
  LoadScene(mSceneManager.Current().empty() ? "main" : mSceneManager.Current());
}

bool Engine::LoadScene(const std::string& name) {
  return mSceneManager.Load(name, mWorld, mAssetManager);
}

void Engine::Render(){
  mWindow.BeginRender();
  mRenderSystem.Render(mWorld, mRenderManager);
  mWindow.EndRender();
}


void Engine::Quit(){
  mIsRunning = false;
}

Engine* Engine::mInstance = nullptr;

Engine& Engine::Instance(){
  if(!mInstance){
    mInstance = new Engine;
  }
  return *mInstance;
}


bool Engine::Initialize() {
  ECLIPSE_ASSERT(!mIsInitialized, "trying to reinitialize engine");
  GetInfo();
  bool flag = false;
  if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
    ECLIPSE_ERROR("Error initializing SDL2 {}", SDL_GetError());
    flag = false;
  } else {
    SDL_version version;
    SDL_VERSION(&version);
    ECLIPSE_INFO(
        "SDL {}.{}.{}",
        (int)version.major,
        (int)version.minor,
        (int)version.patch
    );


      core::WindowProperites props = mApp->GetWindowProperties();
      if(mWindow.Create(props)){
        mRenderManager.Initialize();
        mClock.Initialize();
        mAudioManager.Initialize();
        flag = true;
        mIsInitialized = true;
        mIsRunning = true;
         mSceneManager.Register(
             "main", [this](ecs::World& world, managers::AssetManager& assets) {
               mApp->Initialize(world, assets);
             });
         LoadScene("main");
         mGameStateManager.Set(managers::GameState::Playing);
        input::mouse::Initialize();
        input::keyboard::Initialize();
      }
  }
  return flag;
}

void Engine::Shutdown(){
  mIsInitialized = false;
  mIsRunning = false;
  mApp->Shutdown();
  mWorld.Clear();
  mAssetManager.Clear();
  mRenderManager.Shutdown();
  mAudioManager.Shutdown();
  mWindow.Shutdown();
  mLogManager.Shutdown();
  SDL_Quit();
}

void Engine::GetInfo(){
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
