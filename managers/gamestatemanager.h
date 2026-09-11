#pragma once

namespace eclipse::managers {

enum class GameState {
  Boot,
  Menu,
  Playing,
  Paused,
  GameOver,
};

class GameStateManager {
public:
  GameState Current() const { return mCurrent; }
  GameState Previous() const { return mPrevious; }

  void Set(GameState state) {
    if (state == mCurrent) {
      return;
    }
    mPrevious = mCurrent;
    mCurrent = state;
  }

private:
  GameState mCurrent = GameState::Boot;
  GameState mPrevious = GameState::Boot;
};

} // namespace eclipse::managers
