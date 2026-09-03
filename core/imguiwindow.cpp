#include "imguiwindow.h"
#include <SDL_video.h>
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_opengl3.h>
#include <external/imgui/imgui_impl_sdl2.h>
#include "../src/engine.h"
namespace eclipse::core{

void ImGuiWindow::Create(ImGuiWindowProperties& ImGuiProps){
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigWindowsMoveFromTitleBarOnly = ImGuiProps.MoveFromTitleBarOnly;
  if(ImGuiProps.IsDockingEnabled){
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }
  if(ImGuiProps.IsViewPortEnabled){
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  }
  auto& window = Engine::Instance().GetWindow();
  ImGui_ImplSDL2_InitForOpenGL(window.GetSDLWindow(), window.GetGLContext());
  ImGui_ImplOpenGL3_Init("#version 410");
}
void ImGuiWindow::Shutdown(){
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}
void ImGuiWindow::HandleSDLEvents(SDL_Event &event){
  ImGui_ImplSDL2_ProcessEvent(&event);
}
void ImGuiWindow::BeginRender(){
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}
void ImGuiWindow::EndRender(){
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  auto& window = Engine::Instance().GetWindow();
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
  SDL_GL_MakeCurrent(window.GetSDLWindow(), window.GetGLContext());
}
bool ImGuiWindow::WantCaptureKeyboard(){
  return ImGui::GetIO().WantCaptureKeyboard;
}

bool ImGuiWindow::WantCaptureMouse(){
  return ImGui::GetIO().WantCaptureMouse;
}
}
