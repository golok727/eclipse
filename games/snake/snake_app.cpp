#include "snake_app.h"

#include "../../components/rendercomponents.h"
#include "../../input/keyboard.h"
#include "../../src/engine.h"

#include <external/imgui/imgui.h>

#include <algorithm>
#include <cstdint>

namespace eclipse::games::snake {

core::WindowProperites SnakeApp::GetWindowProperties() {
  core::WindowProperites props;
  props.title = "Eclipse Snake";
  props.w = 960;
  props.h = 720;
  props.wMin = 640;
  props.hMin = 480;
  props.clearColor = {0.02f, 0.02f, 0.03f};
  props.ImGuiProps.IsDockingEnabled = false;
  props.ImGuiProps.IsViewportEnabled = false;
  return props;
}

void SnakeApp::Initialize(ecs::World& world, managers::AssetManager& assets) {
  BuildMesh();

  mShader = assets.GetShader("shader.sprite");
  mFloorTexture = assets.GetTexture("snake.tile.floor");
  mWallTexture = assets.GetTexture("snake.tile.wall");
  mHeadTexture = assets.GetTexture("snake.head");
  mBodyTexture = assets.GetTexture("snake.body");
  mRewardTexture = assets.GetTexture("snake.reward.red");
  mBonusTexture = assets.GetTexture("snake.bonus.blue");

  BuildGrid(world);

  mCameraEntity = world.CreateEntity();
  world.Add<components::Transform>(
      mCameraEntity,
      components::Transform{{
          GridWidth * CellSize * 0.5f,
          GridHeight * CellSize * 0.5f,
      }});
  auto& camera = world.Add<components::Camera>(mCameraEntity);
  camera.fitSize = {GridWidth * CellSize, GridHeight * CellSize};
  camera.fitScale = 1.08f;

  mSnakeEntities.reserve(GridWidth * GridHeight);
  ResetGame(world);
}

void SnakeApp::Update(ecs::World& world, float deltaTime) {
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_ESCAPE)) {
    Engine::Instance().Quit();
    return;
  }

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_R)) {
    ResetGame(world);
    mState = GameState::Playing;
    return;
  }

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_RETURN)) {
    if (mState == GameState::Waiting) {
      mState = GameState::Playing;
    } else if (mState == GameState::GameOver || mState == GameState::Won) {
      ResetGame(world);
      mState = GameState::Playing;
    }
  }

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_P)) {
    if (mState == GameState::Playing) {
      mState = GameState::Paused;
    } else if (mState == GameState::Paused) {
      mState = GameState::Playing;
    }
  }

  if (mState != GameState::Playing) {
    return;
  }

  HandleDirectionInput();

  const float frameTime = std::min(deltaTime, 0.25f);
  if (mBoostTimer > 0.0f) {
    mBoostTimer = std::max(0.0f, mBoostTimer - frameTime);
  }

  if (mBonus.active) {
    mBonusTimer = std::max(0.0f, mBonusTimer - frameTime);
    if (mBonusTimer == 0.0f) {
      ClearPickup(world, mBonus);
    }
  }

  const float stepDelay =
      mBoostTimer > 0.0f ? StepDelayBoost : StepDelayNormal;
  mStepTimer += frameTime;
  while (mStepTimer >= stepDelay && mState == GameState::Playing) {
    mStepTimer -= stepDelay;
    Step(world);
  }
}

void SnakeApp::Shutdown() {
  mStaticTiles.clear();
  mSnakeEntities.clear();
  mSnake.clear();
  mMesh.reset();
  mShader.reset();
  mFloorTexture.reset();
  mWallTexture.reset();
  mHeadTexture.reset();
  mBodyTexture.reset();
  mRewardTexture.reset();
  mBonusTexture.reset();
}

void SnakeApp::ImGuiRender() {
  auto& window = Engine::Instance().GetWindow();
  const auto windowSize = window.GetSize();

  ImGui::SetNextWindowPos({0.0f, 0.0f}, ImGuiCond_Always);
  ImGui::SetNextWindowSize(
      {static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)},
      ImGuiCond_Always);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::Begin(
      "Snake Game View",
      nullptr,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoSavedSettings |
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs);
  ImGui::Image(
      ImTextureID(window.GetFrameBuffer()->GetTextureId()),
      {static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)},
      {0.0f, 1.0f},
      {1.0f, 0.0f});
  ImGui::End();
  ImGui::PopStyleVar(3);

  ImGui::SetNextWindowPos({16.0f, 16.0f}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.82f);
  ImGui::Begin(
      "Snake",
      nullptr,
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs);

  ImGui::Text("Score: %d", mScore);
  ImGui::Text("High score: %d", mHighScore);
  ImGui::Text("Length: %d", static_cast<int>(mSnake.size()));
  if (mBoostTimer > 0.0f) {
    ImGui::TextColored(
        {0.35f, 0.65f, 1.0f, 1.0f}, "Speed boost: %.1fs", mBoostTimer);
  }
  if (mBonus.active) {
    ImGui::Text("Blue bonus expires: %.1fs", mBonusTimer);
  }

  ImGui::Separator();
  ImGui::TextUnformatted("Move: Arrow keys / WASD");
  ImGui::TextUnformatted("Pause: P   Restart: R   Quit: Escape");

  switch (mState) {
  case GameState::Waiting:
    ImGui::Separator();
    ImGui::TextColored(
        {0.45f, 1.0f, 0.65f, 1.0f}, "PRESS ENTER TO START");
    break;
  case GameState::Paused:
    ImGui::Separator();
    ImGui::TextColored({1.0f, 0.85f, 0.3f, 1.0f}, "PAUSED — press P");
    break;
  case GameState::GameOver:
    ImGui::Separator();
    ImGui::TextColored({1.0f, 0.25f, 0.25f, 1.0f}, "GAME OVER");
    ImGui::TextUnformatted("Press Enter or R to restart");
    break;
  case GameState::Won:
    ImGui::Separator();
    ImGui::TextColored({0.4f, 1.0f, 0.7f, 1.0f}, "BOARD CLEARED!");
    ImGui::TextUnformatted("Press Enter or R to play again");
    break;
  case GameState::Playing:
    break;
  }

  ImGui::End();
}

void SnakeApp::BuildMesh() {
  float vertices[]{
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
  };
  uint32_t elements[]{0, 3, 1, 1, 3, 2};
  float texcoords[]{
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
  };
  mMesh = std::make_shared<graphics::Mesh>(
      vertices, 4, 3, texcoords, elements, 6);
}

void SnakeApp::BuildGrid(ecs::World& world) {
  mGrid.width = GridWidth;
  mGrid.height = GridHeight;
  mGrid.blocked.assign(GridWidth * GridHeight, false);

  for (int x = 2; x < 18; ++x) {
    mGrid.blocked[3 * GridWidth + x] = x != 4;
    mGrid.blocked[6 * GridWidth + x] = x != 15;
    mGrid.blocked[9 * GridWidth + x] = x != 7;
  }
  for (int y = 4; y < 9; ++y) {
    mGrid.blocked[y * GridWidth + 11] = true;
  }

  mStaticTiles.reserve(GridWidth * GridHeight);
  for (int y = 0; y < GridHeight; ++y) {
    for (int x = 0; x < GridWidth; ++x) {
      const glm::ivec2 cell{x, y};
      const bool wall = IsBlockedCell(cell);
      mStaticTiles.push_back(CreateSprite(
          world,
          cell,
          CellSize - 2.0f,
          wall ? mWallTexture : mFloorTexture,
          wall ? 5 : 0));
    }
  }
}

void SnakeApp::ResetGame(ecs::World& world) {
  for (const auto entity : mSnakeEntities) {
    world.DestroyEntity(entity);
  }
  mSnakeEntities.clear();
  ClearPickup(world, mReward);
  ClearPickup(world, mBonus);

  mSnake.clear();
  mSnake.push_back({4, 11});
  mSnake.push_back({3, 11});
  mSnake.push_back({2, 11});
  mDirection = {1, 0};
  mQueuedDirection = {1, 0};
  mStepTimer = 0.0f;
  mBoostTimer = 0.0f;
  mBonusTimer = 0.0f;
  mGrow = 0;
  mScore = 0;
  mState = GameState::Waiting;

  SyncSnakeEntities(world);
  if (!SpawnReward(world)) {
    mState = GameState::Won;
  }
}

void SnakeApp::HandleDirectionInput() {
  glm::ivec2 wanted = mQueuedDirection;

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_UP) ||
      input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_W)) {
    wanted = {0, -1};
  } else if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_DOWN) ||
             input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_S)) {
    wanted = {0, 1};
  } else if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_LEFT) ||
             input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_A)) {
    wanted = {-1, 0};
  } else if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_RIGHT) ||
             input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_D)) {
    wanted = {1, 0};
  }

  if (wanted + mDirection != glm::ivec2{0, 0}) {
    mQueuedDirection = wanted;
  }
}

void SnakeApp::Step(ecs::World& world) {
  mDirection = mQueuedDirection;
  const glm::ivec2 newHead = mSnake.front() + mDirection;
  const bool tailWillMove = mGrow == 0;

  if (!mGrid.IsInside(newHead.x, newHead.y) ||
      IsBlockedCell(newHead) ||
      SnakeContains(newHead, tailWillMove)) {
    mState = GameState::GameOver;
    mHighScore = std::max(mHighScore, mScore);
    return;
  }

  mSnake.push_front(newHead);

  if (mReward.active && newHead == mReward.cell) {
    mScore += 10;
    ++mGrow;
    ClearPickup(world, mReward);
    if (SpawnReward(world)) {
      SpawnBonusMaybe(world);
    } else {
      mState = GameState::Won;
    }
  } else if (mBonus.active && newHead == mBonus.cell) {
    mScore += 50;
    mGrow += 2;
    mBoostTimer = BoostDuration;
    mBonusTimer = 0.0f;
    ClearPickup(world, mBonus);
  }

  if (mGrow > 0) {
    --mGrow;
  } else {
    mSnake.pop_back();
  }

  mHighScore = std::max(mHighScore, mScore);
  SyncSnakeEntities(world);
}

void SnakeApp::SyncSnakeEntities(ecs::World& world) {
  while (mSnakeEntities.size() < mSnake.size()) {
    const size_t index = mSnakeEntities.size();
    mSnakeEntities.push_back(CreateSprite(
        world,
        mSnake[index],
        CellSize - 2.0f,
        index == 0 ? mHeadTexture : mBodyTexture,
        index == 0 ? 30 : 25));
  }

  while (mSnakeEntities.size() > mSnake.size()) {
    world.DestroyEntity(mSnakeEntities.back());
    mSnakeEntities.pop_back();
  }

  for (size_t i = 0; i < mSnake.size(); ++i) {
    const auto entity = mSnakeEntities[i];
    auto* transform = world.Get<components::Transform>(entity);
    auto* sprite = world.Get<components::SpriteRenderer>(entity);
    if (!transform || !sprite) {
      continue;
    }

    transform->previousPosition = transform->position;
    transform->position = CellToWorld(mSnake[i]);
    transform->rotation = i == 0 ? HeadRotation() : 0.0f;
    transform->scale = {CellSize - 2.0f, -(CellSize - 2.0f)};
    sprite->texture = i == 0 ? mHeadTexture : mBodyTexture;
    sprite->layer = i == 0 ? 30 : 25;
  }
}

bool SnakeApp::SpawnReward(ecs::World& world) {
  glm::ivec2 cell;
  if (!TryRandomFreeCell(cell)) {
    return false;
  }

  mReward.cell = cell;
  mReward.entity =
      CreateSprite(world, cell, CellSize * 0.45f, mRewardTexture, 40);
  mReward.active = true;
  return true;
}

void SnakeApp::SpawnBonusMaybe(ecs::World& world) {
  if (mBonus.active) {
    return;
  }

  std::uniform_int_distribution<int> chance(1, 100);
  if (chance(mRng) > 25) {
    return;
  }

  glm::ivec2 cell;
  if (!TryRandomFreeCell(cell)) {
    return;
  }

  mBonus.cell = cell;
  mBonus.entity =
      CreateSprite(world, cell, CellSize * 0.75f, mBonusTexture, 45);
  mBonus.active = true;
  mBonusTimer = BonusLifetime;
}

void SnakeApp::ClearPickup(ecs::World& world, Pickup& pickup) {
  if (pickup.active && pickup.entity != ecs::NullEntity) {
    world.DestroyEntity(pickup.entity);
  }
  pickup.entity = ecs::NullEntity;
  pickup.active = false;
}

ecs::Entity SnakeApp::CreateSprite(
    ecs::World& world,
    glm::ivec2 cell,
    float visualScale,
    const std::shared_ptr<graphics::Texture>& texture,
    int layer) {
  const auto entity = world.CreateEntity();
  const glm::vec2 position = CellToWorld(cell);
  world.Add<components::Transform>(
      entity,
      components::Transform{
          position,
          position,
          0.0f,
          {visualScale, -visualScale},
      });
  auto& sprite = world.Add<components::SpriteRenderer>(
      entity,
      components::SpriteRenderer{mMesh, mShader, texture});
  sprite.layer = layer;
  return entity;
}

glm::vec2 SnakeApp::CellToWorld(glm::ivec2 cell) const {
  return {
      cell.x * CellSize + CellSize * 0.5f,
      cell.y * CellSize + CellSize * 0.5f,
  };
}

float SnakeApp::HeadRotation() const {
  if (mDirection == glm::ivec2{0, 1}) {
    return glm::radians(90.0f);
  }
  if (mDirection == glm::ivec2{-1, 0}) {
    return glm::radians(180.0f);
  }
  if (mDirection == glm::ivec2{0, -1}) {
    return glm::radians(-90.0f);
  }
  return 0.0f;
}

bool SnakeApp::IsBlockedCell(glm::ivec2 cell) const {
  return mGrid.IsBlocked(cell.x, cell.y);
}

bool SnakeApp::SnakeContains(glm::ivec2 cell, bool ignoreTail) const {
  auto end = mSnake.end();
  if (ignoreTail && !mSnake.empty()) {
    --end;
  }
  return std::find(mSnake.begin(), end, cell) != end;
}

bool SnakeApp::TryRandomFreeCell(glm::ivec2& result) {
  size_t choices = 0;
  for (int y = 0; y < GridHeight; ++y) {
    for (int x = 0; x < GridWidth; ++x) {
      const glm::ivec2 cell{x, y};
      if (IsBlockedCell(cell) || SnakeContains(cell) ||
          (mReward.active && mReward.cell == cell) ||
          (mBonus.active && mBonus.cell == cell)) {
        continue;
      }

      ++choices;
      std::uniform_int_distribution<size_t> choose(1, choices);
      if (choose(mRng) == 1) {
        result = cell;
      }
    }
  }
  return choices != 0;
}

} // namespace eclipse::games::snake
