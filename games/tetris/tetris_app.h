#pragma once

#include "../../ecs/entity.h"
#include "../../graphics/mesh.h"
#include "../../graphics/shader.h"
#include "../../graphics/texture.h"
#include "../../src/app.h"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

namespace eclipse::games::tetris {

class TetrisApp final : public App {
public:
  core::WindowProperites GetWindowProperties() override;
  void Initialize(ecs::World& world, managers::AssetManager& assets) override;
  void Update(ecs::World& world, float deltaTime) override;
  void Shutdown() override;
  void ImGuiRender() override;

private:
  enum class Tetromino : std::uint8_t { I, O, T, S, Z, J, L, Count };
  enum class GameState { Waiting, Playing, Paused, GameOver };

  struct Cell {
    int x = 0;
    int y = 0;
  };

  struct Piece {
    Tetromino type = Tetromino::I;
    int rotation = 0;
    int x = 4;
    int y = 1;
  };

  static constexpr int BoardWidth = 10;
  static constexpr int BoardHeight = 20;
  static constexpr int ViewWidth = 18;
  static constexpr int ViewHeight = 22;
  static constexpr int BoardOffsetX = 4;
  static constexpr int BoardOffsetY = 1;
  static constexpr float CellSize = 32.0f;
  static constexpr size_t DynamicEntityCount = BoardWidth * BoardHeight + 16;

  using Board = std::array<std::array<std::uint8_t, BoardWidth>, BoardHeight>;

  void BuildMesh();
  void BuildScene(ecs::World& world);
  void NewGame();
  void SpawnNextPiece();
  Tetromino TakeFromBag();
  void RefillBag();
  void HoldPiece();
  bool TryMove(int dx, int dy);
  bool TryRotate(int delta);
  bool CanPlace(const Piece& piece) const;
  void HardDrop();
  void LockPiece();
  int ClearLines();
  int DropDistance() const;
  float FallInterval() const;
  std::array<Cell, 4> PieceCells(const Piece& piece) const;
  std::array<Cell, 4> BaseCells(Tetromino type) const;

  void SyncVisuals(ecs::World& world);
  void EmitBlock(
      ecs::World& world,
      size_t& cursor,
      glm::vec2 position,
      const std::shared_ptr<graphics::Texture>& texture,
      int layer,
      float scale = CellSize - 3.0f);
  ecs::Entity CreateSprite(
      ecs::World& world,
      glm::vec2 position,
      glm::vec2 scale,
      const std::shared_ptr<graphics::Texture>& texture,
      int layer);
  glm::vec2 BoardCellToWorld(int x, int y) const;
  glm::vec2 ViewCellToWorld(float x, float y) const;
  const std::shared_ptr<graphics::Texture>& BlockTexture(Tetromino type) const;
  const char* PieceName(Tetromino type) const;

  Board mBoard{};
  Piece mActive;
  Tetromino mNext = Tetromino::I;
  Tetromino mHold = Tetromino::Count;
  bool mCanHold = true;
  std::array<Tetromino, 7> mBag{};
  size_t mBagIndex = mBag.size();
  float mFallTimer = 0.0f;
  int mScore = 0;
  int mHighScore = 0;
  int mLines = 0;
  int mLevel = 1;
  GameState mState = GameState::Waiting;

  ecs::Entity mCameraEntity = ecs::NullEntity;
  std::vector<ecs::Entity> mStaticEntities;
  std::vector<ecs::Entity> mDynamicEntities;

  std::shared_ptr<graphics::Mesh> mMesh;
  std::shared_ptr<graphics::Shader> mShader;
  std::shared_ptr<graphics::Texture> mBackgroundTexture;
  std::shared_ptr<graphics::Texture> mGridTexture;
  std::shared_ptr<graphics::Texture> mFrameTexture;
  std::array<std::shared_ptr<graphics::Texture>, 7> mBlockTextures;
  std::shared_ptr<graphics::Texture> mGhostTexture;

  std::mt19937 mRng{std::random_device{}()};
};

} // namespace eclipse::games::tetris
