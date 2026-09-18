#include "audiomanager.h"

#include "../src/log.h"

#include <SDL2/SDL.h>

namespace eclipse::managers {

bool AudioManager::Initialize() {
  if (mDevice != 0) {
    return true;
  }

  const SDL_AudioSpec desired = core::AudioClip::EngineFormat();
  mDevice = SDL_OpenAudioDevice(nullptr, 0, &desired, &mDeviceSpec, 0);
  if (mDevice == 0) {
    ECLIPSE_WARN("Unable to open audio device: {}", SDL_GetError());
    return false;
  }
  SDL_PauseAudioDevice(mDevice, 0);
  return true;
}

void AudioManager::Shutdown() {
  if (mDevice == 0) {
    return;
  }
  StopAll();
  SDL_CloseAudioDevice(mDevice);
  mDevice = 0;
  mDeviceSpec = {};
}

bool AudioManager::Play(const std::shared_ptr<core::AudioClip>& clip) {
  if (mDevice == 0 || !clip || !clip->IsLoaded()) {
    return false;
  }

  const auto& format = clip->GetFormat();
  if (format.freq != mDeviceSpec.freq || format.format != mDeviceSpec.format ||
      format.channels != mDeviceSpec.channels) {
    ECLIPSE_ERROR("Audio clip '{}' does not match the engine audio format",
                  clip->GetPath());
    return false;
  }

  const auto& data = clip->GetData();
  if (data.empty() || SDL_QueueAudio(mDevice, data.data(),
                                     static_cast<Uint32>(data.size())) != 0) {
    ECLIPSE_WARN("Unable to queue audio '{}': {}", clip->GetPath(),
                 SDL_GetError());
    return false;
  }
  return true;
}

void AudioManager::StopAll() {
  if (mDevice != 0) {
    SDL_ClearQueuedAudio(mDevice);
  }
}

std::uint32_t AudioManager::GetQueuedBytes() const {
  return mDevice == 0 ? 0 : SDL_GetQueuedAudioSize(mDevice);
}

} // namespace eclipse::managers
