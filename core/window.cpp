#include "window.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../src/engine.h"
#include "../src/log.h"
#include <SDL2/SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_video.h>
#include <glad/glad.h>
#include <memory>
namespace eclipse::core {

WindowProperites::WindowProperites(){
  title = "eclipse";
  x = SDL_WINDOWPOS_CENTERED;
  y = SDL_WINDOWPOS_CENTERED;
  w = 1920;
  h = 1080;
  wMin = 400;
  hMin = 400;
  clearColor = glm::vec3(0,0,255);
  flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
}




Window::Window() : mWindow(nullptr),mGLContext(nullptr){}
Window::~Window() {
  if (mWindow) {
    Shutdown();
  }
}
bool Window::Create(const WindowProperites& props) {
  mWindow = SDL_CreateWindow(
      props.title.c_str(),
      props.x,
      props.y,
      props.w,
      props.h,
      props.flags
  );
  if (!mWindow) {
    ECLIPSE_ERROR("Error creating SDL Windows {}", SDL_GetError());
    return false;
  }

  SetAttributes();

  mGLContext = SDL_GL_CreateContext(mWindow);

  if(!mGLContext){
    ECLIPSE_ERROR("Error Creating SDL GL context");
  }

  if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
    ECLIPSE_ERROR("Error loading gl proc");
  }

   SDL_SetWindowMinimumSize(mWindow, props.wMin, props.hMin);

  mImGuiWindow.Create(props.ImGuiProps);


  mFrameBuffer = std::make_shared<graphics::FrameBuffer>(props.w,props.h);
  glm::vec4 clearColor(props.clearColor.r,props.clearColor.g,props.clearColor.b,1.f);
  mFrameBuffer->SetClearColor(clearColor);
  
  return true;
}

void Window::BeginRender(){
  auto size = GetSize();
  mFrameBuffer->Resize(size.x, size.y);
  auto& rm = Engine::Instance().GetRenderManager();
  rm.Clear();
  rm.Submit(ECLIPSE_SUBMIT_RC(PushFrameBuffer,mFrameBuffer));
}

void Window::EndRender(){

  auto& rm = Engine::Instance().GetRenderManager();
  rm.Submit(ECLIPSE_SUBMIT_RC(PopFrameBuffer));
  rm.Flush();


  mImGuiWindow.BeginRender();
  Engine::Instance().GetApp().ImGuiRender();
  mImGuiWindow.EndRender();
  SDL_GL_SwapWindow(mWindow);
}


void Window::SetAttributes() {

#ifdef ECLIPSE_PLATFORM_MAC
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
  );
#endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  
}

glm::ivec2 Window::GetSize(){
  int w,h;
  SDL_GetWindowSize(mWindow, &w, &h);
  return glm::ivec2(w,h);
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
  }
  if(!mImGuiWindow.WantToCaptureKeyboard()){
    input::keyboard::Update();
  }
  if(!mImGuiWindow.WantToCaptureMouse()){
    input::mouse::Update();
  }
}
void Window::Shutdown() {
  SDL_DestroyWindow(mWindow);
  mWindow = nullptr;
  SDL_GL_DeleteContext(mGLContext);
  mGLContext = nullptr;
}
} // namespace eclipse::core
