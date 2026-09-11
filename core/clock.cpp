#include "clock.h"

#include <SDL2/SDL_timer.h>

#include <algorithm>

namespace eclipse::core {

void Clock::Initialize() {
  mFrequency = SDL_GetPerformanceFrequency();
  mLastCounter = SDL_GetPerformanceCounter();
  mDeltaTime = 0.0f;
  mTotalTime = 0.0f;
}

void Clock::Tick() {
  const auto currentCounter = SDL_GetPerformanceCounter();
  const auto elapsedCounter = currentCounter - mLastCounter;
  mLastCounter = currentCounter;

  if (mFrequency == 0) {
    mDeltaTime = 0.0f;
    return;
  }

  mDeltaTime = std::min(
      static_cast<float>(elapsedCounter) / static_cast<float>(mFrequency),
      0.25f);
  mTotalTime += mDeltaTime;
}

} // namespace eclipse::core
