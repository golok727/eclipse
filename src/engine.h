#pragma once
#include "../core/window.h"
#include "log.h"
#include "app.h"
#include "../managers/rendermanager.h"
namespace eclipse{
  class Engine{
    public:
      static Engine& Instance();
      void Quit();
      void Run(App* app);
      inline App& GetApp(){return* mApp;}
      inline managers::RenderManager& GetRenderManager(){return mRenderManager;}
      inline core::Window& GetWindow(){return mWindow;}

    private:

      void Update();
      void Render();

      App* mApp;

      static Engine* mInstance;
      bool mIsInitialized;
      bool mIsRunning;

      core::Window mWindow;
      managers::LogManager mLogManager;
      managers::RenderManager mRenderManager;
      Engine();
      ~Engine();
      bool Initialize();
      void Shutdown();
      void GetInfo();

  };
}
