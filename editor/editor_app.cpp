#include "editor_app.h"

// LEARNING MAP
// This file is intentionally the most approachable example game in the repo.
// Learn these functions in order:
// 1. GetWindowProperties: choose the application window size.
// 2. Initialize: load files and create entities/components.
// 3. Update: respond to game input each frame.
// 4. ImGuiRender: build tools and debug windows.

#include "../components/gameplaycomponents.h"
#include "../components/rendercomponents.h"
#include "../input/keyboard.h"
#include "../managers/layeredtilemaploader.h"
#include "../src/engine.h"
#include "../src/log.h"

#include <external/imgui/imgui.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

namespace eclipse::editor {

core::WindowProperites EditorApp::GetWindowProperties() {
  core::WindowProperites props;
  props.title = "eclipseeditor";
  props.w = 1280;
  props.h = 900;
  props.wMin = 200;
  props.hMin = 200;
  props.ImGuiProps.IsDockingEnabled = true;
  props.ImGuiProps.IsViewportEnabled = true;
  props.ImGuiProps.MoveFromTitleBarOnly = true;
  return props;
}

void EditorApp::Initialize(ecs::World& world, managers::AssetManager& assets) {
  ECLIPSE_TRACE("EditorApp::Initialize()");

  // The engine's sprite renderer expects one reusable textured quad.
  // Learn next: graphics/mesh.cpp, then graphics/render_system.cpp.
  float vertices[]{0.5f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
                   -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f};
  uint32_t elements[]{0, 3, 1, 1, 3, 2};
  float texcoords[]{1.0f, 1.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f};
  mMesh = std::make_shared<graphics::Mesh>(
      vertices, 4, 3, texcoords, elements, 6);

  // AssetManager caches each path, so loading the same image twice is cheap.
  // Learn next: managers/assetmanager.cpp and graphics/texture.cpp.
  mIdleTexture = assets.LoadTexture("assets/character/Schoolgirl_1/Idle.png");
  mWalkTexture = assets.LoadTexture("assets/character/Schoolgirl_1/Walk.png");
  mAttackTexture = assets.LoadTexture(
      "assets/character/Schoolgirl_1/Attack_1.png");
  mChargeTexture = assets.LoadTexture("assets/character/Schoolgirl_1/Charge.png");
  mHurtTexture = assets.LoadTexture("assets/character/Schoolgirl_1/Hurt.png");
  mDeadTexture = assets.LoadTexture("assets/character/Schoolgirl_1/Dead.png");
  mShader = assets.LoadShaderFromFiles(
      "default-sprite", "assets/shaders/sprite.vert",
      "assets/shaders/sprite.frag");

  // A factory keeps entity creation consistent. The components describe data;
  // systems later decide what that data does.
  auto createSprite = [&world, this](glm::vec2 position, glm::vec2 scale,
                                     bool controlled, int layer) {
    const auto entity = world.CreateEntity();
    world.Add<components::Transform>(
        entity, components::Transform{position, position, 0.0f, scale});
    auto& sprite = world.Add<components::SpriteRenderer>(
        entity, components::SpriteRenderer{mMesh, mShader, mIdleTexture});
    sprite.layer = layer;
    if (controlled) {
      world.Add<components::PlayerController>(entity);
    }
    return entity;
  };

  auto createNpc = [&world, this](glm::vec2 position,
                                  const std::shared_ptr<graphics::Texture>& texture,
                                  int frameCount, float size, int layer) {
    const auto entity = world.CreateEntity();
    world.Add<components::Transform>(
        entity, components::Transform{position, position, 0.0f,
                                      {size, -size}});
    auto& sprite = world.Add<components::SpriteRenderer>(
        entity, components::SpriteRenderer{mMesh, mShader, texture});
    sprite.layer = layer;
    world.Add<components::Animation>(
        entity, components::Animation{frameCount, frameCount, 1, 0.16f});
    world.Add<components::NpcBehavior>(
        entity, components::NpcBehavior{{position.x - 45.0f, position.y},
                                         {position.x + 45.0f, position.y}, 18.0f});
    return entity;
  };

  // Player data: transform, rendering, movement, collision, and animation.
  mPlayerEntity = createSprite({464.0f, 500.0f}, {112.0f, -112.0f}, true, 100);
  world.Add<components::Collider>(
      mPlayerEntity, components::Collider{{24.0f, 24.0f}, {0.0f, 0.0f}, false});
  world.Add<components::Animation>(
      mPlayerEntity, components::Animation{6, 6, 1, 0.12f});

  // The map is currently a set of exported Photoshop layers, not a tile grid.
  // Learn next: managers/layeredtilemaploader.cpp.
  managers::LayeredTilemapLoader mapLoader;
  mapLoader.Load(
      "assets/tilemaps",
      {"Layer_0011_0.png", "Layer_0010_1.png", "Layer_0009_2.png",
       "Layer_0008_3.png", "Layer_0006_4.png", "Layer_0005_5.png",
       "Layer_0003_6.png", "Layer_0002_7.png", "Layer_0001_8.png",
       "Layer_0000_9.png", "Layer_0004_Lights.png", "Layer_0007_Lights.png"},
      world, assets, mMesh, mShader);

  // NPCs are ordinary entities with an Animation and NpcBehavior component.
  createNpc({340.0f, 470.0f}, mWalkTexture, 6, 96.0f, 110);
  createNpc({590.0f, 450.0f}, mAttackTexture, 4, 112.0f, 111);
  createNpc({520.0f, 585.0f}, mChargeTexture, 7, 64.0f, 112);
  createNpc({690.0f, 530.0f}, mHurtTexture, 2, 96.0f, 113);

  // Four invisible solid walls keep the player inside the image.
  auto createBoundary = [&world](glm::vec2 position, glm::vec2 size) {
    const auto entity = world.CreateEntity();
    world.Add<components::Transform>(entity,
                                     components::Transform{position});
    world.Add<components::Collider>(
        entity, components::Collider{size, {0.0f, 0.0f}, true});
  };
  createBoundary({-8.0f, 396.5f}, {16.0f, 793.0f});
  createBoundary({936.0f, 396.5f}, {16.0f, 793.0f});
  createBoundary({464.0f, -8.0f}, {928.0f, 16.0f});
  createBoundary({464.0f, 801.0f}, {928.0f, 16.0f});

  // The camera is an entity too. fitSize keeps the map's proportions correct.
  mCameraEntity = world.CreateEntity();
  world.Add<components::Transform>(
      mCameraEntity, components::Transform{{464.0f, 396.5f}});
  auto& camera = world.Add<components::Camera>(mCameraEntity);
  camera.fitSize = {928.0f, 793.0f};
  camera.fitScale = 1.25f;

  // A small fixed demo grid makes the algorithm visible before an NPC uses it.
  // Learn next: systems/pathfinding.cpp, especially cost and parent arrays.
  mDemoGrid.width = 20;
  mDemoGrid.height = 14;
  mDemoGrid.blocked.assign(mDemoGrid.width * mDemoGrid.height, false);
  for (int x = 2; x < 18; ++x) {
    mDemoGrid.blocked[3 * mDemoGrid.width + x] = x != 4;
    mDemoGrid.blocked[6 * mDemoGrid.width + x] = x != 15;
    mDemoGrid.blocked[9 * mDemoGrid.width + x] = x != 7;
  }
  for (int y = 4; y < 9; ++y) {
    mDemoGrid.blocked[y * mDemoGrid.width + 11] = true;
  }
  mDemoFrames = systems::AStarPathfinder::FindPathTrace(
      mDemoGrid, {1, 1}, {18, 12});
}

void EditorApp::Update(ecs::World& world, float deltaTime) {
  if (!mDemoFrames.empty()) {
    mDemoElapsed += deltaTime;
    if (mDemoElapsed >= 0.08f) {
      mDemoElapsed = 0.0f;
      if (mDemoFrame + 1 < mDemoFrames.size()) {
        ++mDemoFrame;
      }
    }
  }
  // Animation and movement are handled by engine systems. This function only
  // chooses which player animation should be shown.
  auto* sprite = world.Get<components::SpriteRenderer>(mPlayerEntity);
  auto* transform = world.Get<components::Transform>(mPlayerEntity);
  auto* animation = world.Get<components::Animation>(mPlayerEntity);
  if (!sprite || !transform || !animation) {
    return;
  }

  std::shared_ptr<graphics::Texture> texture = mIdleTexture;
  int frameCount = 6;
  float frameSize = 128.0f;
  if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_LEFT) ||
      input::keyboard::Key(input::ECLIPSE_INPUT_KEY_A)) {
    mFacingRight = false;
  } else if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_RIGHT) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_D)) {
    mFacingRight = true;
  }
  if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_SPACE)) {
    texture = mAttackTexture;
    frameCount = 4;
  } else if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_C)) {
    texture = mChargeTexture;
    frameCount = 7;
    frameSize = 64.0f;
  } else if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_H)) {
    texture = mHurtTexture;
    frameCount = 2;
  } else if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_K)) {
    texture = mDeadTexture;
    frameCount = 2;
  } else if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_LEFT) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_RIGHT) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_UP) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_DOWN) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_A) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_D) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_W) ||
             input::keyboard::Key(input::ECLIPSE_INPUT_KEY_S)) {
    texture = mWalkTexture;
  }

  if (sprite->texture != texture) {
    sprite->texture = texture;
    animation->currentFrame = 0;
    animation->elapsed = 0.0f;
  }
  animation->frameCount = frameCount;
  animation->columns = frameCount;
  animation->rows = 1;
  transform->scale = {mFacingRight ? frameSize : -frameSize, -frameSize};
}

void EditorApp::Shutdown() {
  // Release GPU objects before the OpenGL window is destroyed.
  mIdleTexture.reset();
  mWalkTexture.reset();
  mAttackTexture.reset();
  mChargeTexture.reset();
  mHurtTexture.reset();
  mDeadTexture.reset();
  mShader.reset();
  mMesh.reset();
}

void EditorApp::ImGuiRender() {
  // This is editor-only UI. A finished game can replace this with game menus.
  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
  auto& engine = Engine::Instance();
  auto& gameState = engine.GetGameState();

  ImGui::Begin("Game State");
  const auto state = gameState.Current();
  const char* stateName = state == managers::GameState::Playing ? "Playing"
                             : state == managers::GameState::Paused ? "Paused"
                             : state == managers::GameState::Menu ? "Menu"
                             : state == managers::GameState::GameOver ? "Game Over"
                                                                       : "Boot";
  ImGui::Text("State: %s", stateName);
  if (state == managers::GameState::Playing && ImGui::Button("Pause"))
    gameState.Set(managers::GameState::Paused);
  if (state == managers::GameState::Paused && ImGui::Button("Resume"))
    gameState.Set(managers::GameState::Playing);
  if (state != managers::GameState::Menu && ImGui::Button("Main Menu"))
    gameState.Set(managers::GameState::Menu);
  if (state == managers::GameState::Menu && ImGui::Button("Start Game"))
    gameState.Set(managers::GameState::Playing);
  if (state == managers::GameState::Playing && ImGui::Button("Game Over"))
    gameState.Set(managers::GameState::GameOver);
  if (state == managers::GameState::GameOver && ImGui::Button("Restart")) {
    engine.LoadScene("main");
    gameState.Set(managers::GameState::Playing);
  }
  ImGui::End();

  ImGui::Begin("Entities");
  auto& world = engine.GetWorld();
  for (const auto entity : world.GetEntities()) {
    const std::string label = "Entity " + std::to_string(entity);
    if (ImGui::Selectable(label.c_str(), mSelectedEntity == entity))
      mSelectedEntity = entity;
  }
  if (auto* transform = world.Get<components::Transform>(mSelectedEntity)) {
    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::DragFloat2("Position", &transform->position.x, 1.0f);
    ImGui::DragFloat2("Scale", &transform->scale.x, 0.1f, 0.01f, 1000.0f);
  }
  ImGui::End();

  // This canvas is intentionally black so the purple route is easy to see.
  // The same path vector can later drive an NPC's movement target by target.
  ImGui::SetNextWindowSize(ImVec2{620.0f, 500.0f}, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("A* Pathfinding Demo")) {
    const ImVec2 available = ImGui::GetContentRegionAvail();
    const float cell = std::min(
        available.x / static_cast<float>(mDemoGrid.width),
        available.y / static_cast<float>(mDemoGrid.height));
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    auto* draw = ImGui::GetWindowDrawList();
    const ImVec2 canvasEnd = {origin.x + cell * mDemoGrid.width,
                              origin.y + cell * mDemoGrid.height};
    draw->AddRectFilled(origin, canvasEnd, IM_COL32(3, 3, 8, 255));

    for (int y = 0; y < mDemoGrid.height; ++y) {
      for (int x = 0; x < mDemoGrid.width; ++x) {
        const ImVec2 min = {origin.x + x * cell + 1.0f,
                            origin.y + y * cell + 1.0f};
        const ImVec2 max = {origin.x + (x + 1) * cell - 1.0f,
                            origin.y + (y + 1) * cell - 1.0f};
        if (mDemoGrid.IsBlocked(x, y)) {
          draw->AddRectFilled(min, max, IM_COL32(35, 35, 48, 255));
        } else {
          draw->AddRect(min, max, IM_COL32(20, 20, 35, 255));
        }
      }
    }

    const auto& demoFrame = mDemoFrames[mDemoFrame];
    for (const auto cellPosition : demoFrame.visited) {
      const ImVec2 min = {origin.x + cellPosition.x * cell + 3.0f,
                          origin.y + cellPosition.y * cell + 3.0f};
      const ImVec2 max = {origin.x + (cellPosition.x + 1) * cell - 3.0f,
                          origin.y + (cellPosition.y + 1) * cell - 3.0f};
      draw->AddRectFilled(min, max, IM_COL32(70, 30, 110, 255));
    }
    for (const auto cellPosition : demoFrame.frontier) {
      const ImVec2 min = {origin.x + cellPosition.x * cell + 4.0f,
                          origin.y + cellPosition.y * cell + 4.0f};
      const ImVec2 max = {origin.x + (cellPosition.x + 1) * cell - 4.0f,
                          origin.y + (cellPosition.y + 1) * cell - 4.0f};
      draw->AddRectFilled(min, max, IM_COL32(80, 120, 220, 255));
    }
    for (const auto cellPosition : demoFrame.path) {
      const ImVec2 min = {origin.x + cellPosition.x * cell + 3.0f,
                          origin.y + cellPosition.y * cell + 3.0f};
      const ImVec2 max = {origin.x + (cellPosition.x + 1) * cell - 3.0f,
                          origin.y + (cellPosition.y + 1) * cell - 3.0f};
      draw->AddRectFilled(min, max, IM_COL32(170, 65, 255, 255));
    }

    const auto drawMarker = [&](glm::ivec2 position, ImU32 color) {
      const ImVec2 min = {origin.x + position.x * cell + 2.0f,
                          origin.y + position.y * cell + 2.0f};
      const ImVec2 max = {origin.x + (position.x + 1) * cell - 2.0f,
                          origin.y + (position.y + 1) * cell - 2.0f};
      draw->AddRectFilled(min, max, color);
    };
    drawMarker({1, 1}, IM_COL32(60, 220, 120, 255));
    drawMarker({18, 12}, IM_COL32(255, 80, 150, 255));
    ImGui::Dummy({cell * mDemoGrid.width, cell * mDemoGrid.height});
    ImGui::Text("Blue: frontier   Violet: visited   Purple: final route");
    if (demoFrame.finished) {
      ImGui::Text("Path found: %zu steps", demoFrame.path.size());
    } else {
      ImGui::Text("Searching...");
    }
  }
  ImGui::End();

  // The GameView shows the engine framebuffer inside an editor window.
  ImGui::SetNextWindowSize(ImVec2{1000.0f, 760.0f}, ImGuiCond_Always);
  if (ImGui::Begin("GameView")) {
    auto& window = engine.GetWindow();
    const auto windowSize = window.GetSize();
    const ImVec2 available = ImGui::GetContentRegionAvail();
    const float aspect = static_cast<float>(windowSize.x) /
                         static_cast<float>(windowSize.y);
    ImVec2 size = available;
    if (available.x / available.y > aspect)
      size.x = available.y * aspect;
    else
      size.y = available.x / aspect;
    const ImVec2 cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPos({cursor.x + (available.x - size.x) * 0.5f,
                         cursor.y + (available.y - size.y) * 0.5f});
    ImGui::Image(ImTextureID(window.GetFrameBuffer()->GetTextureId()), size,
                 {0, 1}, {1, 0});
  }
  ImGui::End();
}

} // namespace eclipse::editor
