#include "../managers/assetmanager.h"

#include <chrono>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <string>

namespace {

int failures = 0;

void Check(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

std::filesystem::path MakeTemporaryDirectory() {
  const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
  const auto path = std::filesystem::temp_directory_path() /
                    ("eclipse-assets-" + std::to_string(suffix));
  std::filesystem::create_directories(path);
  return path;
}

void WriteManifest(const std::filesystem::path& root,
                   const std::string& document) {
  std::ofstream file(root / "manifest.json");
  file << document;
}

void WriteTestWav(const std::filesystem::path& path) {
  constexpr std::array<std::uint8_t, 52> wav{
      'R', 'I', 'F', 'F', 44,  0,   0,   0,   'W', 'A', 'V', 'E', 'f',
      'm', 't', ' ',  16,   0,   0,   0,   1,   0,   1,   0,   64,  31,
      0,   0,   128,  62,   0,   0,   2,   0,   16,  0,   'd', 'a', 't',
      'a', 8,   0,    0,    0,   0,   232, 3,   24,  252, 0,   0};
  std::ofstream file(path, std::ios::binary);
  file.write(reinterpret_cast<const char*>(wav.data()),
             static_cast<std::streamsize>(wav.size()));
}

void TestProjectManifest() {
  eclipse::managers::AssetManager assets;
  Check(assets.Initialize("assets"), "project manifest should validate");
  Check(assets.GetIssues().empty(), "project manifest should have no issues");
  Check(assets.HasAsset("shader.sprite"), "sprite shader should be registered");
  Check(assets.HasAsset("character.schoolgirl1.idle"),
        "player idle texture should be registered");

  const auto resolved =
      assets.ResolveAssetPath("character.schoolgirl1.idle");
  Check(std::filesystem::is_regular_file(resolved),
        "texture ID should resolve to an existing file");
  Check(assets.ResolveAssetPath("shader.sprite").empty(),
        "shader should not resolve as a single-path asset");
}

void TestDuplicateIdsAreRejected() {
  const auto root = MakeTemporaryDirectory();
  std::ofstream(root / "one.png").put('x');
  WriteManifest(
      root,
      R"({"version":1,"assets":[
        {"id":"duplicate","type":"texture","path":"one.png"},
        {"id":"duplicate","type":"texture","path":"one.png"}
      ]})");

  eclipse::managers::AssetManager assets;
  Check(!assets.Initialize(root), "duplicate IDs should invalidate a manifest");
  Check(!assets.GetIssues().empty(), "duplicate IDs should report an issue");
  std::filesystem::remove_all(root);
}

void TestPathsCannotEscapeRoot() {
  const auto root = MakeTemporaryDirectory();
  WriteManifest(
      root,
      R"({"version":1,"assets":[
        {"id":"escape","type":"texture","path":"../outside.png"}
      ]})");

  eclipse::managers::AssetManager assets;
  Check(!assets.Initialize(root), "escaping paths should invalidate a manifest");
  Check(assets.ResolveAssetPath("escape").empty(),
        "escaping paths should never resolve");
  std::filesystem::remove_all(root);
}

void TestAudioIsDecodedAndCached() {
  const auto root = MakeTemporaryDirectory();
  WriteTestWav(root / "tone.wav");
  WriteManifest(
      root,
      R"({"version":1,"assets":[
        {"id":"audio.tone","type":"audio","path":"tone.wav"}
      ]})");

  eclipse::managers::AssetManager assets;
  Check(assets.Initialize(root), "audio manifest should validate");
  const auto first = assets.GetAudioClip("audio.tone");
  const auto second = assets.GetAudioClip("audio.tone");
  Check(first && first->IsLoaded(), "WAV data should decode");
  Check(first == second, "audio clips should be cached by asset ID");
  Check(first && !first->GetData().empty(), "decoded audio should contain data");
  Check(first && first->GetFormat().freq == 48000,
        "audio should convert to the engine sample rate");
  std::filesystem::remove_all(root);
}

void TestUnknownTypesAreRejected() {
  const auto root = MakeTemporaryDirectory();
  WriteManifest(
      root,
      R"({"version":1,"assets":[
        {"id":"bad","type":"model","path":"model.obj"}
      ]})");

  eclipse::managers::AssetManager assets;
  Check(!assets.Initialize(root), "unknown asset types should be rejected");
  Check(!assets.HasAsset("bad"), "invalid entries should not be registered");
  std::filesystem::remove_all(root);
}

} // namespace

int main() {
  TestProjectManifest();
  TestDuplicateIdsAreRejected();
  TestPathsCannotEscapeRoot();
  TestUnknownTypesAreRejected();
  TestAudioIsDecodedAndCached();
  if (failures != 0) {
    std::cerr << failures << " asset test(s) failed\n";
    return 1;
  }
  std::cout << "All asset manager tests passed\n";
  return 0;
}
