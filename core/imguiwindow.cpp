#include "imguiwindow.h"

#include "../src/engine.h"
#include "../src/log.h"

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_opengl3.h>
#include <external/imgui/imgui_impl_sdl2.h>

namespace eclipse::core {

bool ImGuiWindow::Create(const ImGuiWindowProperties& properties) {
  if (mCreated) {
    return true;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigWindowsMoveFromTitleBarOnly = properties.MoveFromTitleBarOnly;
  if (properties.IsDockingEnabled) {
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }
  if (properties.IsViewportEnabled) {
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  }

  auto& window = Engine::Instance().GetWindow();
  if (!ImGui_ImplSDL2_InitForOpenGL(window.GetSDLWindow(),
                                    window.GetGLContext())) {
    ECLIPSE_ERROR("Unable to initialize the ImGui SDL backend");
    ImGui::DestroyContext();
    return false;
  }
  if (!ImGui_ImplOpenGL3_Init("#version 410")) {
    ECLIPSE_ERROR("Unable to initialize the ImGui OpenGL backend");
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    return false;
  }

  mCreated = true;
  return true;
}

void ImGuiWindow::Shutdown() {
  if (!mCreated) {
    return;
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  mCreated = false;
}

void ImGuiWindow::HandleSDLEvents(SDL_Event& event) {
  if (mCreated) {
    ImGui_ImplSDL2_ProcessEvent(&event);
  }
}

void ImGuiWindow::BeginRender() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void ImGuiWindow::EndRender() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGuiIO& io = ImGui::GetIO();
  if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
    auto& window = Engine::Instance().GetWindow();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(window.GetSDLWindow(), window.GetGLContext());
  }
}

bool ImGuiWindow::WantToCaptureKeyboard() const {
  return mCreated && ImGui::GetIO().WantCaptureKeyboard;
}

bool ImGuiWindow::WantToCaptureMouse() const {
  return mCreated && ImGui::GetIO().WantCaptureMouse;
}

} // namespace eclipse::core
