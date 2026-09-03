#include "window.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../src/engine.h"
#include "../src/log.h"
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_video.h>
namespace eclipse::core {

WindowProperties::WindowProperties(){
  title = "eclipse";
  x = SDL_WINDOWPOS_CENTERED;
  y = SDL_WINDOWPOS_CENTERED;
  w = 1920;
  h = 1080;
  flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  wMin = 200;
  hMin = 200;
  ccR = 0;
  ccG = 0;
  ccB = 0;
}


Window::Window() : mWindow(nullptr) {}
Window::~Window() {
  if (mWindow) {
    Shutdown();
  }
}
bool Window::Create(WindowProperties& props) {
  mWindow = SDL_CreateWindow(props.title.c_str(), props.x, props.y, props.w, props.h, props.flags);
  if (!mWindow) {
    ECLIPSE_ERROR("Error creating SDL Window {}", SDL_GetError());
    return false;
  }
#ifdef ECLIPSE_PLATFORM_MAC
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetWindowMinimumSize(mWindow, props.wMin, props.hMin);



  mGLContext = SDL_GL_CreateContext(mWindow);
  if (!mGLContext) {
    ECLIPSE_ERROR("Error Creating GL context {}", SDL_GetError());
    return false;
  }

  mImGuiWindow.Create(props.ImGuiProps);

  gladLoadGLLoader(SDL_GL_GetProcAddress);

  // glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LEQUAL);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  // glClearColor(0,0,255,255);

  Engine::Instance().GetRenderManager().SetClearColor(props.ccR, props.ccG, props.ccB, 1);

  return true;
}

void Window::GetSize(int &w, int &h) { SDL_GetWindowSize(mWindow, &w, &h); }

void Window::BeginRender() {
  // glClear(GL_DEPTH_BUFFER_BIT |GL_COLOR_BUFFER_BIT);
  Engine::Instance().GetRenderManager().Clear();
}

void Window::EndRender() {
  mImGuiWindow.BeginRender();
  Engine::Instance().GetApp().ImGuiRender();
  mImGuiWindow.EndRender();

  SDL_GL_SwapWindow(mWindow);
}

void Window::PollEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    mImGuiWindow.HandleSDLEvents(event);
    switch (event.type) {
    case SDL_QUIT:
      Engine::Instance().Quit();
    default:
      break;
    }

    if (!mImGuiWindow.WantCaptureKeyboard()) {

      input::keyboard::Update();
    }
    if (!mImGuiWindow.WantCaptureMouse()) {
      input::mouse::Update();
    }
  }
}

void Window::Shutdown() {
  SDL_DestroyWindow(mWindow);
  mImGuiWindow.Shutdown();
  mWindow = nullptr;
  mGLContext = nullptr;
}
} // namespace eclipse::core
