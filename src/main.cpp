#include "main.h"
#include "../graphics/mesh.h"
#include "../graphics/shader.h"
#include "../src/log.h"
#include "app.h"
#include <external/imgui/imgui.h>
#include <memory>
#include "../input/keyboard.h"
#include "../input/mouse.h"
#include "../graphics/texture.h"
#include "../components/rendercomponents.h"
#include "../components/gameplaycomponents.h"
#include "../managers/tiledmaploader.h"
using namespace eclipse;

class Editor : public App {
private:
  std::shared_ptr<graphics::Mesh> mMesh;
  std::shared_ptr<graphics::Shader> mShader;
  std::shared_ptr<graphics::Texture>mTexture;
  ecs::Entity mDestroyableEntity = ecs::NullEntity;
  ecs::Entity mPlayerEntity = ecs::NullEntity;
  ecs::Entity mCameraEntity = ecs::NullEntity;
  ecs::Entity mSelectedEntity = ecs::NullEntity;

public:


  core::WindowProperites GetWindowProperties()override {
    core::WindowProperites props;
    props.title = "eclipseeditor";
    props.w = 800;
    props.h = 600;
    props.wMin = 200;
    props.hMin = 200;
    props.ImGuiProps.IsDockingEnabled = true;
    props.ImGuiProps.IsViewportEnabled = true;
    props.ImGuiProps.MoveFromTitleBarOnly = true;
    
    return props;
  }



  void Initialize(ecs::World& world, managers::AssetManager& assets) override {

    ECLIPSE_TRACE("Editor:: Initialize()");
    float vertices[]{// -0.5f  ,-0.5f  ,0.f,
                     // 0.f    ,0.5f   ,0.f,
                     // 0.5f   ,-0.5f  ,0.f

                     0.5f,
                     0.5f,
                     0.f,

                     0.5f,
                     -0.5f,
                     0.f,

                     -0.5f,
                     -0.5f,
                     0.f,

                     -0.5f,
                     0.5f,
                     0.f

    };

    uint32_t elements[]{0, 3, 1, 1, 3, 2};

    float texcoords[]{1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};

    mMesh = std::make_shared<eclipse::graphics::Mesh>(
        &vertices[0],
        4,
        3,
        &texcoords[0],
        &elements[0],
        6
    );

    mTexture = assets.LoadTexture(
        "assets/Tiled_files/Citizen1_Walk.png");
    mShader = assets.LoadShaderFromFiles(
        "default-sprite",
        "assets/shaders/sprite.vert",
        "assets/shaders/sprite.frag");

    auto createSprite = [&world, this](glm::vec2 position, glm::vec2 scale,
                                       bool controlled, int layer) -> ecs::Entity {
      auto entity = world.CreateEntity();
      world.Add<components::Transform>(
          entity, components::Transform{position, position, 0.0f, scale});
      auto& sprite = world.Add<components::SpriteRenderer>(
          entity, components::SpriteRenderer{mMesh, mShader, mTexture});
      sprite.layer = layer;
      if (controlled) {
        world.Add<components::PlayerController>(entity);
      }
      return entity;
    };

    mPlayerEntity = createSprite({128.0f, 192.0f}, {32.0f, 32.0f}, true, 100);
    world.Add<components::Collider>(
        mPlayerEntity,
        components::Collider{{24.0f, 24.0f}, {0.0f, 0.0f}, false});
    world.Add<components::Animation>(
        mPlayerEntity, components::Animation{24, 6, 4, 0.12f});

    managers::TiledMapLoader mapLoader;
    mapLoader.Load("assets/Tiled_files/Exterior.tmx", world, assets, mMesh,
                   mShader);

    auto createBoundary = [&world](glm::vec2 position, glm::vec2 size) {
      const auto entity = world.CreateEntity();
      world.Add<components::Transform>(entity,
                                       components::Transform{position});
      world.Add<components::Collider>(
          entity, components::Collider{size, {0.0f, 0.0f}, true});
    };
    createBoundary({-8.0f, 192.0f}, {16.0f, 400.0f});
    createBoundary({264.0f, 192.0f}, {16.0f, 400.0f});
    createBoundary({128.0f, -8.0f}, {256.0f, 16.0f});
    createBoundary({128.0f, 392.0f}, {256.0f, 16.0f});

    mCameraEntity = world.CreateEntity();
    world.Add<components::Transform>(mCameraEntity);
    auto& cameraComponent = world.Add<components::Camera>(mCameraEntity);
    cameraComponent.followTarget = mPlayerEntity;
  }
  void Update(ecs::World& world, float) override {

    // ECLIPSE_TRACE("Editor:: Update()");

    auto windowSize = Engine::Instance().GetWindow().GetSize();

    // engine::Instance().GetWindow().GetSize(windowWidth, windowHeight);
    float xNormal = (float)input::mouse::X() / (float)windowSize.x;
    float yNormal =
        (float)(windowSize.y- input::mouse::Y()) / (float)windowSize.y;

    mShader->SetUniformFloat3("color", 1, 1, 1);

    if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_SPACE) &&
        world.IsAlive(mDestroyableEntity)) {
      world.DestroyEntity(mDestroyableEntity);
      ECLIPSE_INFO("Destroyed test entity {}", mDestroyableEntity);
    }
    if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_F6)) {
      Engine::Instance().GetAudioManager().PlayWav("assets/audio_test.wav");
    }
    // Mouse controls the color gradient; keyboard controls the rectangle
    // position.
    (void)xNormal;
    (void)yNormal;


    // ECLIPSE_TRACE("{},{}", windowSize.x, windowSize.y);
    // ECLIPSE_TRACE(
    //     "X: {}, Y: {}, {}{}{}{}{}",
    //     input::mouse::X(),
    //     input::mouse::Y(),

    //     input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_LEFT),

    //     input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_RIGHT),

    //     input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_MIDDLE),

    //     input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_X1),

    //     input::mouse::Button(input::ECLIPSE_INPUT_MOUSE_X2)
    // );

    

  }

  void Shutdown() override {
    // Release OpenGL resources while the context is still alive.
    mTexture.reset();
    mShader.reset();
    mMesh.reset();
  }


void ImGuiRender() override {
  ImGui::DockSpaceOverViewport(0,ImGui::GetMainViewport());

   ImGui::Begin("eclipse");
   auto& world = Engine::Instance().GetWorld();
   auto* playerTransform = world.Get<components::Transform>(mPlayerEntity);
   auto* cameraTransform = world.Get<components::Transform>(mCameraEntity);
   if (playerTransform && cameraTransform) {
     ImGui::Text("Player: %.1f, %.1f", playerTransform->position.x,
                 playerTransform->position.y);
     ImGui::Text("Camera: %.1f, %.1f", cameraTransform->position.x,
                 cameraTransform->position.y);
   }
   ImGui::End();
   ImGui::Begin("Entities");
   if (ImGui::Button("Create Empty Entity")) {
     mSelectedEntity = world.CreateEntity();
     world.Add<components::Transform>(mSelectedEntity,
                                      components::Transform{{128.0f, 192.0f}});
   }
   ImGui::SameLine();
   if (ImGui::Button("Reload Scene")) {
     Engine::Instance().ReloadScene();
     ImGui::End();
     return;
   }
   if (ImGui::Button("Play Test Sound")) {
     Engine::Instance().GetAudioManager().PlayWav("assets/audio_test.wav");
   }
   for (const auto entity : world.GetEntities()) {
     const std::string label = "Entity " + std::to_string(entity);
     if (ImGui::Selectable(label.c_str(), mSelectedEntity == entity)) {
       mSelectedEntity = entity;
     }
   }
   if (auto* transform = world.Get<components::Transform>(mSelectedEntity)) {
     ImGui::Separator();
     ImGui::Text("Transform");
     ImGui::DragFloat2("Position", &transform->position.x, 1.0f);
     ImGui::DragFloat2("Scale", &transform->scale.x, 0.1f, 0.01f, 1000.0f);
   }
   if (auto* collider = world.Get<components::Collider>(mSelectedEntity)) {
     ImGui::Separator();
     ImGui::Text("Collider");
     ImGui::DragFloat2("Size", &collider->size.x, 1.0f, 0.0f, 1000.0f);
     ImGui::Checkbox("Static", &collider->isStatic);
     if (ImGui::Button("Remove Collider")) {
       world.Remove<components::Collider>(mSelectedEntity);
     }
   } else if (world.IsAlive(mSelectedEntity) &&
              ImGui::Button("Add Collider")) {
     world.Add<components::Collider>(
         mSelectedEntity,
         components::Collider{{16.0f, 16.0f}, {0.0f, 0.0f}, false});
   }
   if (world.IsAlive(mSelectedEntity) &&
       ImGui::Button("Destroy Selected Entity")) {
     world.DestroyEntity(mSelectedEntity);
     mSelectedEntity = ecs::NullEntity;
   }
   ImGui::End();
  ImGui::Begin("elcipsev2");
  ImGui::End();

  if (ImGui::Begin("GameView")) {

      auto &window = Engine::Instance().GetWindow();
      ImVec2 size = {480,360};
      ImVec2 uv0 ={0,1};
      ImVec2 uv1 ={1,0};
      ImGui::Image(
        
        ImTextureID(window.GetFrameBuffer()->GetTextureId()),size,uv0,uv1
      );
    }
    ImGui::End();
  
}

};


eclipse::App *CreateApp() { return new Editor; }
