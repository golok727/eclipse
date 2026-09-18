#include "tetris_app.h"

#include "../../components/rendercomponents.h"
#include "../../input/keyboard.h"
#include "../../src/engine.h"

#include <external/imgui/imgui.h>

#include <algorithm>

namespace eclipse::games::tetris {

core::WindowProperites TetrisApp::GetWindowProperties() {
  core::WindowProperites props;
  props.title = "Eclipse Tetris";
  props.w = 720;
  props.h = 600;
  props.wMin = 600;
  props.hMin = 500;
  props.clearColor = {0.01f, 0.015f, 0.04f};
  props.ImGuiProps.IsDockingEnabled = false;
  props.ImGuiProps.IsViewportEnabled = false;
  return props;
}

void TetrisApp::Initialize(ecs::World& world, managers::AssetManager& assets) {
  BuildMesh();
  mShader = assets.GetShader("shader.sprite");
  mBackgroundTexture = assets.GetTexture("tetris.background");
  mGridTexture = assets.GetTexture("tetris.grid");
  mFrameTexture = assets.GetTexture("tetris.frame");
  mBlockTextures = {
      assets.GetTexture("tetris.block.i"),
      assets.GetTexture("tetris.block.o"),
      assets.GetTexture("tetris.block.t"),
      assets.GetTexture("tetris.block.s"),
      assets.GetTexture("tetris.block.z"),
      assets.GetTexture("tetris.block.j"),
      assets.GetTexture("tetris.block.l"),
  };
  mGhostTexture = assets.GetTexture("tetris.ghost");

  BuildScene(world);
  NewGame();
  SyncVisuals(world);
}

void TetrisApp::Update(ecs::World& world, float deltaTime) {
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_ESCAPE)) {
    Engine::Instance().Quit();
    return;
  }

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_R)) {
    NewGame();
    mState = GameState::Playing;
  }

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_RETURN)) {
    if (mState == GameState::Waiting) {
      mState = GameState::Playing;
    } else if (mState == GameState::GameOver) {
      NewGame();
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
    SyncVisuals(world);
    return;
  }

  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_LEFT) ||
      input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_A)) {
    TryMove(-1, 0);
  }
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_RIGHT) ||
      input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_D)) {
    TryMove(1, 0);
  }
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_UP) ||
      input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_X)) {
    TryRotate(1);
  }
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_Z)) {
    TryRotate(-1);
  }
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_C)) {
    HoldPiece();
  }
  if (input::keyboard::KeyDown(input::ECLIPSE_INPUT_KEY_SPACE)) {
    HardDrop();
    SyncVisuals(world);
    return;
  }

  const bool softDropping =
      input::keyboard::Key(input::ECLIPSE_INPUT_KEY_DOWN) ||
      input::keyboard::Key(input::ECLIPSE_INPUT_KEY_S);
  const float interval = softDropping ? 0.045f : FallInterval();
  mFallTimer += std::min(deltaTime, 0.25f);

  while (mFallTimer >= interval && mState == GameState::Playing) {
    mFallTimer -= interval;
    if (TryMove(0, 1)) {
      if (softDropping) {
        ++mScore;
      }
    } else {
      LockPiece();
      break;
    }
  }

  mHighScore = std::max(mHighScore, mScore);
  SyncVisuals(world);
}

void TetrisApp::Shutdown() {
  mStaticEntities.clear();
  mDynamicEntities.clear();
  mMesh.reset();
  mShader.reset();
  mBackgroundTexture.reset();
  mGridTexture.reset();
  mFrameTexture.reset();
  for (auto& texture : mBlockTextures) {
    texture.reset();
  }
  mGhostTexture.reset();
}

void TetrisApp::ImGuiRender() {
  auto& window = Engine::Instance().GetWindow();
  const auto windowSize = window.GetSize();
  const float width = static_cast<float>(windowSize.x);
  const float height = static_cast<float>(windowSize.y);

  ImGui::SetNextWindowPos({0.0f, 0.0f}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({width, height}, ImGuiCond_Always);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::Begin(
      "Tetris Game View",
      nullptr,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoSavedSettings |
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs);
  ImGui::Image(
      ImTextureID(window.GetFrameBuffer()->GetTextureId()),
      {width, height},
      {0.0f, 1.0f},
      {1.0f, 0.0f});
  ImGui::End();
  ImGui::PopStyleVar(3);

  const ImGuiWindowFlags overlayFlags =
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs;

  ImGui::SetNextWindowPos({width * 0.5f - 95.0f, 12.0f}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.72f);
  ImGui::Begin("Tetris Title", nullptr, overlayFlags);
  ImGui::TextColored({0.45f, 0.85f, 1.0f, 1.0f}, "E C L I P S E  T E T R I S");
  ImGui::End();

  ImGui::SetNextWindowPos({16.0f, 72.0f}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.72f);
  ImGui::Begin("Hold Label", nullptr, overlayFlags);
  ImGui::TextColored({0.72f, 0.68f, 1.0f, 1.0f}, "HOLD");
  ImGui::Text("%s", mHold == Tetromino::Count ? "—" : PieceName(mHold));
  ImGui::End();

  ImGui::SetNextWindowPos({std::max(16.0f, width - 190.0f), 72.0f}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.72f);
  ImGui::Begin("Next Label", nullptr, overlayFlags);
  ImGui::TextColored({0.72f, 0.68f, 1.0f, 1.0f}, "NEXT");
  ImGui::Text("%s", PieceName(mNext));
  ImGui::Separator();
  ImGui::Text("Score  %07d", mScore);
  ImGui::Text("Best   %07d", mHighScore);
  ImGui::Text("Lines  %d", mLines);
  ImGui::Text("Level  %d", mLevel);
  ImGui::End();

  ImGui::SetNextWindowPos({16.0f, std::max(16.0f, height - 142.0f)}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.72f);
  ImGui::Begin("Tetris Controls", nullptr, overlayFlags);
  ImGui::TextUnformatted("Move     A/D or arrows");
  ImGui::TextUnformatted("Rotate   Up/X · Z reverse");
  ImGui::TextUnformatted("Drop     Down/S · Space hard");
  ImGui::TextUnformatted("Hold     C");
  ImGui::TextUnformatted("Pause P  Restart R  Quit Esc");
  ImGui::End();

  if (mState != GameState::Playing) {
    ImGui::SetNextWindowPos(
        {width * 0.5f, height * 0.5f},
        ImGuiCond_Always,
        {0.5f, 0.5f});
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGui::Begin("Tetris State", nullptr, overlayFlags);
    if (mState == GameState::Waiting) {
      ImGui::TextColored({0.4f, 1.0f, 0.75f, 1.0f}, "PRESS ENTER TO START");
    } else if (mState == GameState::Paused) {
      ImGui::TextColored({1.0f, 0.82f, 0.3f, 1.0f}, "PAUSED — PRESS P");
    } else {
      ImGui::TextColored({1.0f, 0.3f, 0.4f, 1.0f}, "GAME OVER");
      ImGui::TextUnformatted("Press Enter or R to restart");
    }
    ImGui::End();
  }
}

void TetrisApp::BuildMesh() {
  float vertices[]{
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
  };
  std::uint32_t elements[]{0, 3, 1, 1, 3, 2};
  float texcoords[]{
      1.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
  };
  mMesh = std::make_shared<graphics::Mesh>(
      vertices, 4, 3, texcoords, elements, 6);
}

void TetrisApp::BuildScene(ecs::World& world) {
  const glm::vec2 viewCenter = {
      ViewWidth * CellSize * 0.5f,
      ViewHeight * CellSize * 0.5f,
  };
  mStaticEntities.push_back(CreateSprite(
      world,
      viewCenter,
      {ViewWidth * CellSize, -(ViewHeight * CellSize)},
      mBackgroundTexture,
      -20));

  for (int y = 0; y < BoardHeight; ++y) {
    for (int x = 0; x < BoardWidth; ++x) {
      mStaticEntities.push_back(CreateSprite(
          world,
          BoardCellToWorld(x, y),
          {CellSize - 1.0f, -(CellSize - 1.0f)},
          mGridTexture,
          0));
    }
  }

  for (int y = BoardOffsetY - 1; y <= BoardOffsetY + BoardHeight; ++y) {
    for (const int x : {BoardOffsetX - 1, BoardOffsetX + BoardWidth}) {
      mStaticEntities.push_back(CreateSprite(
          world,
          ViewCellToWorld(static_cast<float>(x), static_cast<float>(y)),
          {CellSize - 1.0f, -(CellSize - 1.0f)},
          mFrameTexture,
          5));
    }
  }
  for (int x = BoardOffsetX; x < BoardOffsetX + BoardWidth; ++x) {
    for (const int y : {BoardOffsetY - 1, BoardOffsetY + BoardHeight}) {
      mStaticEntities.push_back(CreateSprite(
          world,
          ViewCellToWorld(static_cast<float>(x), static_cast<float>(y)),
          {CellSize - 1.0f, -(CellSize - 1.0f)},
          mFrameTexture,
          5));
    }
  }

  mDynamicEntities.reserve(DynamicEntityCount);
  for (size_t i = 0; i < DynamicEntityCount; ++i) {
    mDynamicEntities.push_back(CreateSprite(
        world, {0.0f, 0.0f}, {0.0f, 0.0f}, mBlockTextures[0], 20));
  }

  mCameraEntity = world.CreateEntity();
  world.Add<components::Transform>(
      mCameraEntity, components::Transform{viewCenter});
  auto& camera = world.Add<components::Camera>(mCameraEntity);
  camera.fitSize = {ViewWidth * CellSize, ViewHeight * CellSize};
  camera.fitScale = 1.0f;
}

void TetrisApp::NewGame() {
  for (auto& row : mBoard) {
    row.fill(0);
  }
  mScore = 0;
  mLines = 0;
  mLevel = 1;
  mFallTimer = 0.0f;
  mHold = Tetromino::Count;
  mCanHold = true;
  mBagIndex = mBag.size();
  mNext = TakeFromBag();
  SpawnNextPiece();
  mState = GameState::Waiting;
}

void TetrisApp::SpawnNextPiece() {
  mActive = {mNext, 0, BoardWidth / 2 - 1, 1};
  mNext = TakeFromBag();
  mCanHold = true;
  mFallTimer = 0.0f;
  if (!CanPlace(mActive)) {
    mState = GameState::GameOver;
    mHighScore = std::max(mHighScore, mScore);
  }
}

TetrisApp::Tetromino TetrisApp::TakeFromBag() {
  if (mBagIndex >= mBag.size()) {
    RefillBag();
  }
  return mBag[mBagIndex++];
}

void TetrisApp::RefillBag() {
  mBag = {
      Tetromino::I,
      Tetromino::O,
      Tetromino::T,
      Tetromino::S,
      Tetromino::Z,
      Tetromino::J,
      Tetromino::L,
  };
  std::shuffle(mBag.begin(), mBag.end(), mRng);
  mBagIndex = 0;
}

void TetrisApp::HoldPiece() {
  if (!mCanHold) {
    return;
  }

  const Tetromino outgoing = mActive.type;
  if (mHold == Tetromino::Count) {
    mHold = outgoing;
    SpawnNextPiece();
  } else {
    mActive = {mHold, 0, BoardWidth / 2 - 1, 1};
    mHold = outgoing;
    mFallTimer = 0.0f;
    if (!CanPlace(mActive)) {
      mState = GameState::GameOver;
      mHighScore = std::max(mHighScore, mScore);
    }
  }
  mCanHold = false;
}

bool TetrisApp::TryMove(int dx, int dy) {
  Piece candidate = mActive;
  candidate.x += dx;
  candidate.y += dy;
  if (!CanPlace(candidate)) {
    return false;
  }
  mActive = candidate;
  return true;
}

bool TetrisApp::TryRotate(int delta) {
  if (mActive.type == Tetromino::O) {
    return true;
  }

  Piece candidate = mActive;
  candidate.rotation = (candidate.rotation + delta + 4) % 4;
  constexpr std::array<Cell, 6> kicks{{
      {0, 0}, {-1, 0}, {1, 0}, {-2, 0}, {2, 0}, {0, -1},
  }};
  for (const auto kick : kicks) {
    candidate.x = mActive.x + kick.x;
    candidate.y = mActive.y + kick.y;
    if (CanPlace(candidate)) {
      mActive = candidate;
      return true;
    }
  }
  return false;
}

bool TetrisApp::CanPlace(const Piece& piece) const {
  for (const auto cell : PieceCells(piece)) {
    if (cell.x < 0 || cell.x >= BoardWidth || cell.y >= BoardHeight) {
      return false;
    }
    if (cell.y >= 0 && mBoard[cell.y][cell.x] != 0) {
      return false;
    }
  }
  return true;
}

void TetrisApp::HardDrop() {
  const int distance = DropDistance();
  mActive.y += distance;
  mScore += distance * 2;
  LockPiece();
}

void TetrisApp::LockPiece() {
  for (const auto cell : PieceCells(mActive)) {
    if (cell.y < 0) {
      mState = GameState::GameOver;
      mHighScore = std::max(mHighScore, mScore);
      return;
    }
    mBoard[cell.y][cell.x] =
        static_cast<std::uint8_t>(mActive.type) + 1;
  }

  const int cleared = ClearLines();
  constexpr std::array<int, 5> lineScores{{0, 100, 300, 500, 800}};
  mScore += lineScores[cleared] * mLevel;
  mLines += cleared;
  mLevel = mLines / 10 + 1;
  mHighScore = std::max(mHighScore, mScore);
  SpawnNextPiece();
}

int TetrisApp::ClearLines() {
  int writeRow = BoardHeight - 1;
  int cleared = 0;

  for (int readRow = BoardHeight - 1; readRow >= 0; --readRow) {
    const bool full = std::all_of(
        mBoard[readRow].begin(),
        mBoard[readRow].end(),
        [](std::uint8_t value) { return value != 0; });
    if (full) {
      ++cleared;
      continue;
    }
    if (writeRow != readRow) {
      mBoard[writeRow] = mBoard[readRow];
    }
    --writeRow;
  }

  while (writeRow >= 0) {
    mBoard[writeRow].fill(0);
    --writeRow;
  }
  return cleared;
}

int TetrisApp::DropDistance() const {
  Piece candidate = mActive;
  int distance = 0;
  while (true) {
    ++candidate.y;
    if (!CanPlace(candidate)) {
      return distance;
    }
    ++distance;
  }
}

float TetrisApp::FallInterval() const {
  return std::max(0.08f, 0.75f - static_cast<float>(mLevel - 1) * 0.055f);
}

std::array<TetrisApp::Cell, 4> TetrisApp::PieceCells(
    const Piece& piece) const {
  auto cells = BaseCells(piece.type);
  const int rotations = piece.type == Tetromino::O ? 0 : piece.rotation;
  for (int turn = 0; turn < rotations; ++turn) {
    for (auto& cell : cells) {
      const int oldX = cell.x;
      cell.x = -cell.y;
      cell.y = oldX;
    }
  }
  for (auto& cell : cells) {
    cell.x += piece.x;
    cell.y += piece.y;
  }
  return cells;
}

std::array<TetrisApp::Cell, 4> TetrisApp::BaseCells(Tetromino type) const {
  switch (type) {
  case Tetromino::I:
    return {{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}};
  case Tetromino::O:
    return {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}};
  case Tetromino::T:
    return {{{-1, 0}, {0, 0}, {1, 0}, {0, 1}}};
  case Tetromino::S:
    return {{{0, 0}, {1, 0}, {-1, 1}, {0, 1}}};
  case Tetromino::Z:
    return {{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}};
  case Tetromino::J:
    return {{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}};
  case Tetromino::L:
    return {{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}};
  case Tetromino::Count:
    break;
  }
  return {};
}

void TetrisApp::SyncVisuals(ecs::World& world) {
  size_t cursor = 0;

  for (int y = 0; y < BoardHeight; ++y) {
    for (int x = 0; x < BoardWidth; ++x) {
      const auto value = mBoard[y][x];
      if (value == 0) {
        continue;
      }
      EmitBlock(
          world,
          cursor,
          BoardCellToWorld(x, y),
          BlockTexture(static_cast<Tetromino>(value - 1)),
          20);
    }
  }

  const int ghostDistance = DropDistance();
  if (ghostDistance > 0) {
    Piece ghost = mActive;
    ghost.y += ghostDistance;
    for (const auto cell : PieceCells(ghost)) {
      if (cell.y >= 0) {
        EmitBlock(
            world, cursor, BoardCellToWorld(cell.x, cell.y), mGhostTexture, 24);
      }
    }
  }

  for (const auto cell : PieceCells(mActive)) {
    if (cell.y >= 0) {
      EmitBlock(
          world,
          cursor,
          BoardCellToWorld(cell.x, cell.y),
          BlockTexture(mActive.type),
          30);
    }
  }

  // The right sidebar is narrower than a four-cell I piece. Keep preview
  // cells compact and centered so no tetromino touches the board frame.
  constexpr float PreviewSpacing = 0.85f;
  constexpr float NextPreviewX = 15.9f;
  constexpr float NextPreviewY = 5.0f;
  constexpr float PreviewBlockSize = 20.0f;
  const auto nextCells = BaseCells(mNext);
  for (const auto cell : nextCells) {
    EmitBlock(
        world,
        cursor,
        ViewCellToWorld(
            NextPreviewX + static_cast<float>(cell.x) * PreviewSpacing,
            NextPreviewY + static_cast<float>(cell.y) * PreviewSpacing),
        BlockTexture(mNext),
        30,
        PreviewBlockSize);
  }

  if (mHold != Tetromino::Count) {
    const auto holdCells = BaseCells(mHold);
    for (const auto cell : holdCells) {
      EmitBlock(
          world,
          cursor,
          ViewCellToWorld(1.5f + cell.x, 5.0f + cell.y),
          BlockTexture(mHold),
          30,
          CellSize - 5.0f);
    }
  }

  while (cursor < mDynamicEntities.size()) {
    if (auto* transform =
            world.Get<components::Transform>(mDynamicEntities[cursor])) {
      transform->scale = {0.0f, 0.0f};
    }
    ++cursor;
  }
}

void TetrisApp::EmitBlock(
    ecs::World& world,
    size_t& cursor,
    glm::vec2 position,
    const std::shared_ptr<graphics::Texture>& texture,
    int layer,
    float scale) {
  if (cursor >= mDynamicEntities.size()) {
    return;
  }

  const auto entity = mDynamicEntities[cursor++];
  auto* transform = world.Get<components::Transform>(entity);
  auto* sprite = world.Get<components::SpriteRenderer>(entity);
  if (!transform || !sprite) {
    return;
  }

  transform->previousPosition = transform->position;
  transform->position = position;
  transform->scale = {scale, -scale};
  sprite->texture = texture;
  sprite->layer = layer;
}

ecs::Entity TetrisApp::CreateSprite(
    ecs::World& world,
    glm::vec2 position,
    glm::vec2 scale,
    const std::shared_ptr<graphics::Texture>& texture,
    int layer) {
  const auto entity = world.CreateEntity();
  world.Add<components::Transform>(
      entity, components::Transform{position, position, 0.0f, scale});
  auto& sprite = world.Add<components::SpriteRenderer>(
      entity, components::SpriteRenderer{mMesh, mShader, texture});
  sprite.layer = layer;
  return entity;
}

glm::vec2 TetrisApp::BoardCellToWorld(int x, int y) const {
  return ViewCellToWorld(
      static_cast<float>(BoardOffsetX + x),
      static_cast<float>(BoardOffsetY + y));
}

glm::vec2 TetrisApp::ViewCellToWorld(float x, float y) const {
  return {
      x * CellSize + CellSize * 0.5f,
      y * CellSize + CellSize * 0.5f,
  };
}

const std::shared_ptr<graphics::Texture>& TetrisApp::BlockTexture(
    Tetromino type) const {
  const auto index = static_cast<size_t>(type);
  return mBlockTextures[index < mBlockTextures.size() ? index : 0];
}

const char* TetrisApp::PieceName(Tetromino type) const {
  switch (type) {
  case Tetromino::I:
    return "I";
  case Tetromino::O:
    return "O";
  case Tetromino::T:
    return "T";
  case Tetromino::S:
    return "S";
  case Tetromino::Z:
    return "Z";
  case Tetromino::J:
    return "J";
  case Tetromino::L:
    return "L";
  case Tetromino::Count:
    return "—";
  }
  return "?";
}

} // namespace eclipse::games::tetris
