#include "main.h"
#include "../graphics/mesh.h"
#include "../graphics/shader.h"
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "app.h"
#include <external/imgui/imgui.h>
#include <memory>
using namespace eclipse;

class Editor : public App {

private:
  std::shared_ptr<graphics::Mesh> mMesh;
  std::shared_ptr<graphics::Shader> mShader;

  float xKeyOffset = 0.f;
  float yKeyOffset = 0.f;
  float keySpeed = 0.01f;

public:

  core::WindowProperties GetWindowProperties() override {
    core::WindowProperties props;
    props.title = "eclipseEditor";
    props.w = 800;
    props.h = 600;
    props.ImGuiProps.IsDockingEnabled = true;
    props.ImGuiProps.IsViewPortEnabled = true;
    return props;
  }





  void Initialize() override  {
    ECLIPSE_TRACE("Editor::Initialize()")
    float vertices[]{// -0.5f  ,-0.5f  ,0.f,
                     // 0.f    ,0.5f   ,0.f,
                     // 0.5f   ,-0.5f  ,0.f

                     0.5f,  0.5f,
                     0.f,

                     0.5f,  -0.5f,
                     0.f,

                     -0.5f, -0.5f,
                     0.f,

                     -0.5f, 0.5f,
                     0.f

    };

    uint32_t elements[]{0, 3, 1, 1, 3, 2};

    mMesh =
        std::make_shared<eclipse::graphics::Mesh>(&vertices[0], 4, 3,
                                                  &elements[0], 6);

    const char *vertexShader = R"(
            #version 410 core
            layout(location = 0) in vec3 position;
            out vec3 vertexpos;
             uniform vec2 offset = vec2 (0.5);
             void main(){
               vertexpos = position + vec3(offset , 0);
               gl_Position = vec4(position,1.0); 
             }
         )";

    const char *fragmentShader = R"(
            #version 410 core
            out vec4 outColor;
            in vec3 vertexpos;
             void main(){
               outColor = vec4(vertexpos,1.0);
            }
         )";

     mShader =
        std::make_shared<eclipse::graphics::Shader>(vertexShader,
                                                    fragmentShader);
  }
  void ImGuiRender()override {
    ImGui::Begin("eclipse");
    ImGui::End();
    ImGui::Begin("eclipsev2");
    ImGui::End();
    
  }
  void Shutdown() override {}
  void Update() override  {

    ECLIPSE_DEBUG("Editor:: Update");
    int windowWidth = 0;
    int windowHeight = 0;

    Engine::Instance().GetWindow().GetSize(windowWidth, windowHeight);

    float xNormal = (float)input::mouse::X() / (float)windowWidth;
    float yNormal =
        (float)(windowHeight - input::mouse::Y()) / (float)windowHeight;



    if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_UP)) {
      yKeyOffset += keySpeed;
    }
    if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_DOWN)) {
      yKeyOffset -= keySpeed;
    }
    if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_LEFT)) {
      xKeyOffset -= keySpeed * 100;
    }
    if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_RIGHT)) {
      xKeyOffset += keySpeed * 100;
    }

    mShader->SetUniformFloat2("offset", xNormal + xKeyOffset,
                            yNormal + yKeyOffset);

    ECLIPSE_TRACE("{},{}", windowWidth, windowHeight);
    ECLIPSE_TRACE("X: {}, Y: {}, {}{}{}{}{}", input::mouse::X(),
                  input::mouse::Y(),

                  input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_LEFT),

                  input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_RIGHT),

                  input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_MIDDLE),

                  input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_X1),

                  input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_X2));

    
  }
  void Render() override  {

    ECLIPSE_TRACE("Editor::Render");

      auto rc = std::make_unique<graphics::rendercommands::RenderMesh>(mMesh,mShader);
      Engine::Instance().GetRenderManager().Submit(std::move(rc));
      Engine::Instance().GetRenderManager().Flush();

    
  }
};

eclipse::App *CreateApp() { return new Editor; }
