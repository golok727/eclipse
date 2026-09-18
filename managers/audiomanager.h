#pragma once

#include "../core/audioclip.h"

#include <SDL2/SDL_audio.h>
#include <cstdint>

#include <memory>

namespace eclipse::managers {

class AudioManager {
public:
  bool Initialize();
  void Shutdown();
  bool Play(const std::shared_ptr<core::AudioClip>& clip);
  void StopAll();
  bool IsAvailable() const { return mDevice != 0; }
  std::uint32_t GetQueuedBytes() const;

private:
  SDL_AudioDeviceID mDevice = 0;
  SDL_AudioSpec mDeviceSpec{};
};

} // namespace eclipse::managers
