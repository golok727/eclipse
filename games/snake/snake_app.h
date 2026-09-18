#pragma once

#include "../../ecs/entity.h"
#include "../../graphics/mesh.h"
#include "../../graphics/shader.h"
#include "../../graphics/texture.h"
#include "../../src/app.h"
#include "../../systems/pathfinding.h"

#include <glm/glm.hpp>

#include <deque>
#include <memory>
#include <random>
#include <vector>

namespace eclipse::games::snake {

class SnakeApp final : public App {
public:
  core::WindowProperites GetWindowProperties() override;
  void Initialize(ecs::World& world, managers::AssetManager& assets) override;
  void Update(ecs::World& world, float deltaTime) override;
  void Shutdown() override;
  void ImGuiRender() override;

private:
  enum class GameState { Waiting, Playing, Paused, GameOver, Won };

  struct Pickup {
    glm::ivec2 cell{0, 0};
    ecs::Entity entity = ecs::NullEntity;
    bool active = false;
  };

  static constexpr int GridWidth = 20;
  static constexpr int GridHeight = 14;
  static constexpr float CellSize = 40.0f;
  static constexpr float StepDelayNormal = 0.16f;
  static constexpr float StepDelayBoost = 0.09f;
  static constexpr float BonusLifetime = 5.0f;
  static constexpr float BoostDuration = 4.0f;

  void BuildMesh();
  void BuildGrid(ecs::World& world);
  void ResetGame(ecs::World& world);
  void HandleDirectionInput();
  void Step(ecs::World& world);
  void SyncSnakeEntities(ecs::World& world);
  bool SpawnReward(ecs::World& world);
  void SpawnBonusMaybe(ecs::World& world);
  void ClearPickup(ecs::World& world, Pickup& pickup);

  ecs::Entity CreateSprite(
      ecs::World& world,
      glm::ivec2 cell,
      float visualScale,
      const std::shared_ptr<graphics::Texture>& texture,
      int layer);
  glm::vec2 CellToWorld(glm::ivec2 cell) const;
  float HeadRotation() const;
  bool IsBlockedCell(glm::ivec2 cell) const;
  bool SnakeContains(glm::ivec2 cell, bool ignoreTail = false) const;
  bool TryRandomFreeCell(glm::ivec2& result);

  systems::PathGrid mGrid;
  std::deque<glm::ivec2> mSnake;
  Pickup mReward;
  Pickup mBonus;
  glm::ivec2 mDirection{1, 0};
  glm::ivec2 mQueuedDirection{1, 0};
  float mStepTimer = 0.0f;
  float mBoostTimer = 0.0f;
  float mBonusTimer = 0.0f;
  int mGrow = 0;
  int mScore = 0;
  int mHighScore = 0;
  GameState mState = GameState::Waiting;

  std::vector<ecs::Entity> mStaticTiles;
  std::vector<ecs::Entity> mSnakeEntities;
  ecs::Entity mCameraEntity = ecs::NullEntity;

  std::shared_ptr<graphics::Mesh> mMesh;
  std::shared_ptr<graphics::Shader> mShader;
  std::shared_ptr<graphics::Texture> mFloorTexture;
  std::shared_ptr<graphics::Texture> mWallTexture;
  std::shared_ptr<graphics::Texture> mHeadTexture;
  std::shared_ptr<graphics::Texture> mBodyTexture;
  std::shared_ptr<graphics::Texture> mRewardTexture;
  std::shared_ptr<graphics::Texture> mBonusTexture;

  std::mt19937 mRng{std::random_device{}()};
};

} // namespace eclipse::games::snake
