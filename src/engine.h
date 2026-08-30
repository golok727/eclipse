#pragma once
#include "log.h"
#include "../core/window.h"
#include "app.h"
#include "../managers/rendermanager.h"
namespace eclipse{
  class engine{
    public:
      static engine& Instance();
      void Quit();
      inline managers::RenderManager& GetRenderManager(){return mRenderManager;}
      inline core::window& GetWindow(){return mWindow;}
    private:
      static engine* mInstance;
      core::window mWindow;
      managers::LogManager mLogManager;
      managers::RenderManager mRenderManager;
      bool mIsInitialized;
      bool mIsRuning;
      engine();
      ~engine();
      bool Initialize();
      void Shutdown();
      void GetInfo();
  };
}
