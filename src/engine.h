#pragma once
#include "../core/window.h"
#include "../managers/rendermanager.h"
#include "../managers/logmanager.h"
#include "../managers/assetmanager.h"
#include "../managers/audiomanager.h"
#include "app.h"
#include "../systems/render_system.h"
#include "../systems/movement_system.h"
#include "../systems/camera_system.h"
#include "../systems/animation_system.h"
#include "../systems/collision_system.h"
#include "../core/clock.h"
namespace eclipse{
  class Engine{
    public:
      void Run(App* app);
      void Quit();
      void ReloadScene();
      static Engine& Instance();
      inline core::Window& GetWindow(){return mWindow;}
      inline App& GetApp(){return* mApp;}
      inline managers::RenderManager& GetRenderManager(){return mRenderManager;}
      inline ecs::World& GetWorld(){return mWorld;}
      inline managers::AssetManager& GetAssetManager(){return mAssetManager;}
      inline managers::AudioManager& GetAudioManager(){return mAudioManager;}
    private:

      void Update();
      void Render();
      static Engine* mInstance;
      bool mIsInitialized;
      bool mIsRunning;
      managers::RenderManager mRenderManager;
      ecs::World mWorld;
      systems::RenderSystem mRenderSystem;
      systems::MovementSystem mMovementSystem;
      systems::CameraSystem mCameraSystem;
      systems::AnimationSystem mAnimationSystem;
      systems::CollisionSystem mCollisionSystem;
      core::Clock mClock;
      core::Window mWindow;
      App* mApp;
      managers::LogManager mLogManager;
      managers::AssetManager mAssetManager;
      managers::AudioManager mAudioManager;
      Engine();
      ~Engine();
      bool Initialize();
      void Shutdown();
      void GetInfo();

  };
}
