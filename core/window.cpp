#include "window.h"

#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../src/engine.h"
#include "../src/log.h"

#include <glad/glad.h>

namespace eclipse::core {

WindowProperites::WindowProperites() {
  title = "eclipse";
  x = SDL_WINDOWPOS_CENTERED;
  y = SDL_WINDOWPOS_CENTERED;
  w = 1920;
  h = 1080;
  wMin = 400;
  hMin = 400;
  clearColor = glm::vec3(0, 0, 255);
  flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
}

Window::Window() : mWindow(nullptr), mGLContext(nullptr) {}

Window::~Window() { Shutdown(); }

bool Window::Create(const WindowProperites& properties) {
  if (mWindow) {
    ECLIPSE_ERROR("Cannot create an already initialized window");
    return false;
  }

  SetAttributes();
  mWindow = SDL_CreateWindow(properties.title.c_str(), properties.x,
                             properties.y, properties.w, properties.h,
                             properties.flags);
  if (!mWindow) {
    ECLIPSE_ERROR("Error creating SDL window: {}", SDL_GetError());
    return false;
  }

  mGLContext = SDL_GL_CreateContext(mWindow);
  if (!mGLContext) {
    ECLIPSE_ERROR("Error creating SDL GL context: {}", SDL_GetError());
    Shutdown();
    return false;
  }
  if (SDL_GL_MakeCurrent(mWindow, mGLContext) != 0) {
    ECLIPSE_ERROR("Error activating SDL GL context: {}", SDL_GetError());
    Shutdown();
    return false;
  }
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    ECLIPSE_ERROR("Error loading OpenGL functions");
    Shutdown();
    return false;
  }
  if (SDL_GL_SetSwapInterval(1) != 0) {
    ECLIPSE_WARN("Unable to enable vertical sync: {}", SDL_GetError());
  }

  SDL_SetWindowMinimumSize(mWindow, properties.wMin, properties.hMin);
  if (!mImGuiWindow.Create(properties.ImGuiProps)) {
    Shutdown();
    return false;
  }

  mFrameBuffer = std::make_shared<graphics::FrameBuffer>(properties.w,
                                                          properties.h);
  if (!mFrameBuffer->IsValid()) {
    ECLIPSE_ERROR("Unable to create the game framebuffer");
    Shutdown();
    return false;
  }
  mFrameBuffer->SetClearColor(
      {properties.clearColor.r, properties.clearColor.g,
       properties.clearColor.b, 1.0f});
  return true;
}

void Window::BeginRender() {
  const auto size = GetSize();
  if (size.x > 0 && size.y > 0) {
    mFrameBuffer->Resize(static_cast<std::uint32_t>(size.x),
                         static_cast<std::uint32_t>(size.y));
  }
  auto& renderManager = Engine::Instance().GetRenderManager();
  renderManager.Clear();
  renderManager.Submit(ECLIPSE_SUBMIT_RC(PushFrameBuffer, mFrameBuffer));
}

void Window::EndRender() {
  auto& renderManager = Engine::Instance().GetRenderManager();
  renderManager.Submit(ECLIPSE_SUBMIT_RC(PopFrameBuffer));
  renderManager.Flush();

  mImGuiWindow.BeginRender();
  Engine::Instance().GetApp().ImGuiRender();
  mImGuiWindow.EndRender();
  SDL_GL_SwapWindow(mWindow);
}

void Window::SetAttributes() {
#ifdef ECLIPSE_PLATFORM_MAC
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

glm::ivec2 Window::GetSize() {
  int width = 0;
  int height = 0;
  if (mWindow) {
    SDL_GetWindowSize(mWindow, &width, &height);
  }
  return {width, height};
}

void Window::PollEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    mImGuiWindow.HandleSDLEvents(event);
    if (event.type == SDL_QUIT) {
      Engine::Instance().Quit();
    }
  }

  input::keyboard::Update();
  input::mouse::Update();
  input::keyboard::SetEnabled(!mImGuiWindow.WantToCaptureKeyboard());
  input::mouse::SetEnabled(!mImGuiWindow.WantToCaptureMouse());
}

void Window::Shutdown() {
  mImGuiWindow.Shutdown();
  mFrameBuffer.reset();
  if (mGLContext) {
    SDL_GL_DeleteContext(mGLContext);
    mGLContext = nullptr;
  }
  if (mWindow) {
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
  }
}

} // namespace eclipse::core
