#pragma once

#include <cstdint>

namespace eclipse::core {

class Clock {
public:
  void Initialize();
  void Tick();

  float GetDeltaTime() const { return mDeltaTime; }
  float GetTotalTime() const { return mTotalTime; }

private:
  std::uint64_t mLastCounter = 0;
  std::uint64_t mFrequency = 0;
  float mDeltaTime = 0.0f;
  float mTotalTime = 0.0f;
};

} // namespace eclipse::core
