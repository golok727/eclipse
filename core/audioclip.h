#pragma once

#include <SDL2/SDL_audio.h>

#include <cstdint>
#include <string>
#include <vector>

namespace eclipse::core {

class AudioClip {
public:
  explicit AudioClip(const std::string& path);

  AudioClip(const AudioClip&) = delete;
  AudioClip& operator=(const AudioClip&) = delete;
  AudioClip(AudioClip&&) noexcept = default;
  AudioClip& operator=(AudioClip&&) noexcept = default;

  bool IsLoaded() const { return mLoaded; }
  const std::string& GetPath() const { return mPath; }
  const SDL_AudioSpec& GetFormat() const { return mFormat; }
  const std::vector<std::uint8_t>& GetData() const { return mData; }
  float GetDurationSeconds() const;

  static SDL_AudioSpec EngineFormat();

private:
  std::string mPath;
  SDL_AudioSpec mFormat{};
  std::vector<std::uint8_t> mData;
  bool mLoaded = false;
};

} // namespace eclipse::core
