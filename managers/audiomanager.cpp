#include "audiomanager.h"

#include "../src/log.h"

namespace eclipse::managers {

bool AudioManager::Initialize() {
  // The device is opened lazily when the first WAV file is played.
  return true;
}

void AudioManager::Shutdown() {
  if (mDevice != 0) {
    SDL_ClearQueuedAudio(mDevice);
    SDL_CloseAudioDevice(mDevice);
    mDevice = 0;
  }
}

bool AudioManager::PlayWav(const std::string& path) {
  SDL_AudioSpec wavSpec{};
  Uint8* data = nullptr;
  Uint32 length = 0;
  if (!SDL_LoadWAV(path.c_str(), &wavSpec, &data, &length)) {
    ECLIPSE_WARN("Unable to load audio '{}'", path);
    return false;
  }

  if (mDevice == 0) {
    mDevice = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, &mDeviceSpec, 0);
    if (mDevice == 0) {
      ECLIPSE_WARN("Unable to open audio device: {}", SDL_GetError());
      SDL_FreeWAV(data);
      return false;
    }
    SDL_PauseAudioDevice(mDevice, 0);
  }

  const int result = SDL_QueueAudio(mDevice, data, length);
  SDL_FreeWAV(data);
  if (result != 0) {
    ECLIPSE_WARN("Unable to queue audio '{}': {}", path, SDL_GetError());
    return false;
  }
  return true;
}

} // namespace eclipse::managers
