#pragma once

#include <SDL2/SDL_audio.h>

#include <string>

namespace eclipse::managers {

class AudioManager {
public:
  bool Initialize();
  void Shutdown();
  bool PlayWav(const std::string& path);

private:
  SDL_AudioDeviceID mDevice = 0;
  SDL_AudioSpec mDeviceSpec{};
};

} // namespace eclipse::managers
