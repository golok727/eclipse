#include "audioclip.h"

#include "../src/log.h"

#include <SDL2/SDL.h>

#include <cstring>
#include <limits>

namespace eclipse::core {

SDL_AudioSpec AudioClip::EngineFormat() {
  SDL_AudioSpec format{};
  format.freq = 48000;
  format.format = AUDIO_F32SYS;
  format.channels = 2;
  format.samples = 4096;
  return format;
}

AudioClip::AudioClip(const std::string& path) : mPath(path), mFormat(EngineFormat()) {
  SDL_AudioSpec sourceFormat{};
  Uint8* sourceData = nullptr;
  Uint32 sourceLength = 0;
  if (!SDL_LoadWAV(path.c_str(), &sourceFormat, &sourceData, &sourceLength)) {
    ECLIPSE_WARN("Unable to load audio '{}': {}", path, SDL_GetError());
    return;
  }

  SDL_AudioCVT converter{};
  const int conversion = SDL_BuildAudioCVT(
      &converter, sourceFormat.format, sourceFormat.channels, sourceFormat.freq,
      mFormat.format, mFormat.channels, mFormat.freq);
  if (conversion < 0) {
    ECLIPSE_WARN("Unable to prepare audio conversion for '{}': {}", path,
                 SDL_GetError());
    SDL_FreeWAV(sourceData);
    return;
  }

  if (conversion == 0) {
    mData.assign(sourceData, sourceData + sourceLength);
    SDL_FreeWAV(sourceData);
    mLoaded = true;
    return;
  }

  if (converter.len_mult <= 0 ||
      sourceLength > static_cast<Uint32>(std::numeric_limits<int>::max())) {
    ECLIPSE_WARN("Audio '{}' is too large to convert", path);
    SDL_FreeWAV(sourceData);
    return;
  }

  const auto convertedCapacity =
      static_cast<std::size_t>(sourceLength) * converter.len_mult;
  mData.resize(convertedCapacity);
  std::memcpy(mData.data(), sourceData, sourceLength);
  SDL_FreeWAV(sourceData);

  converter.buf = mData.data();
  converter.len = static_cast<int>(sourceLength);
  if (SDL_ConvertAudio(&converter) < 0) {
    ECLIPSE_WARN("Unable to convert audio '{}': {}", path, SDL_GetError());
    mData.clear();
    return;
  }

  mData.resize(static_cast<std::size_t>(converter.len_cvt));
  mLoaded = true;
}

float AudioClip::GetDurationSeconds() const {
  if (!mLoaded || mFormat.freq <= 0 || mFormat.channels == 0) {
    return 0.0f;
  }
  const int bitsPerSample = SDL_AUDIO_BITSIZE(mFormat.format);
  if (bitsPerSample <= 0) {
    return 0.0f;
  }
  const auto bytesPerFrame =
      static_cast<std::size_t>(mFormat.channels) * bitsPerSample / 8;
  return bytesPerFrame == 0
             ? 0.0f
             : static_cast<float>(mData.size()) /
                   static_cast<float>(bytesPerFrame * mFormat.freq);
}

} // namespace eclipse::core
