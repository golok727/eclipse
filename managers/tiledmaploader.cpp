#include "tiledmaploader.h"

#include "../components/rendercomponents.h"
#include "../components/gameplaycomponents.h"
#include "../src/log.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string Attribute(const std::string& text, const std::string& name) {
  const std::string key = name + "=\"";
  const auto start = text.find(key);
  if (start == std::string::npos) {
    return {};
  }
  const auto valueStart = start + key.size();
  const auto valueEnd = text.find('"', valueStart);
  return valueEnd == std::string::npos
             ? std::string{}
             : text.substr(valueStart, valueEnd - valueStart);
}

int IntegerAttribute(const std::string& text, const std::string& name,
                     int fallback = 0) {
  const auto value = Attribute(text, name);
  return value.empty() ? fallback : std::stoi(value);
}

struct Tileset {
  int firstGid = 0;
  int columns = 1;
  int tileWidth = 16;
  int tileHeight = 16;
  std::shared_ptr<eclipse::graphics::Texture> texture;
};

std::string DirectoryOf(const std::string& path) {
  const auto slash = path.find_last_of("/\\");
  return slash == std::string::npos ? std::string{} : path.substr(0, slash);
}

std::vector<unsigned int> ParseCsv(const std::string& csv) {
  std::vector<unsigned int> values;
  std::stringstream stream(csv);
  std::string value;
  while (std::getline(stream, value, ',')) {
    if (!value.empty()) {
      values.push_back(static_cast<unsigned int>(std::stoul(value)));
    }
  }
  return values;
}

} // namespace

namespace eclipse::managers {

bool TiledMapLoader::Load(
    const std::string& mapPath, ecs::World& world, AssetManager& assets,
    const std::shared_ptr<graphics::Mesh>& mesh,
    const std::shared_ptr<graphics::Shader>& shader) {
  std::ifstream file(mapPath);
  if (!file) {
    ECLIPSE_ERROR("Unable to open Tiled map '{}'", mapPath);
    return false;
  }

  std::stringstream contents;
  contents << file.rdbuf();
  const std::string document = contents.str();
  const std::string assetDirectory = DirectoryOf(mapPath);

  std::vector<Tileset> tilesets;
  std::size_t tilesetPosition = 0;
  while ((tilesetPosition = document.find("<tileset", tilesetPosition)) !=
         std::string::npos) {
    const auto headerEnd = document.find('>', tilesetPosition);
    const auto tilesetEnd = document.find("</tileset>", headerEnd);
    if (headerEnd == std::string::npos || tilesetEnd == std::string::npos) {
      break;
    }

    const auto header = document.substr(tilesetPosition,
                                       headerEnd - tilesetPosition + 1);
    const auto body = document.substr(headerEnd + 1, tilesetEnd - headerEnd);
    const auto imageStart = body.find("<image");
    const auto imageEnd = body.find('>', imageStart);
    if (imageStart != std::string::npos && imageEnd != std::string::npos) {
      const auto image = body.substr(imageStart, imageEnd - imageStart + 1);
      const auto source = Attribute(image, "source");
      if (!source.empty()) {
        Tileset tileset;
        tileset.firstGid = IntegerAttribute(header, "firstgid");
        tileset.columns = IntegerAttribute(header, "columns", 1);
        tileset.tileWidth = IntegerAttribute(header, "tilewidth", 16);
        tileset.tileHeight = IntegerAttribute(header, "tileheight", 16);
        tileset.texture = assets.LoadTextureFile(
            std::filesystem::path(assetDirectory) / source,
            eclipse::graphics::TextureFilter::Nearest);
        tilesets.push_back(std::move(tileset));
      }
    }
    tilesetPosition = tilesetEnd + 10;
  }

  if (tilesets.empty()) {
    ECLIPSE_ERROR("Tiled map '{}' contains no embedded tilesets", mapPath);
    return false;
  }

  int layerIndex = 0;
  std::size_t layerPosition = 0;
  while ((layerPosition = document.find("<layer", layerPosition)) !=
         std::string::npos) {
    const auto headerEnd = document.find('>', layerPosition);
    const auto dataStart = document.find("<data encoding=\"csv\">", headerEnd);
    const auto dataEnd = document.find("</data>", dataStart);
    const auto layerEnd = document.find("</layer>", dataEnd);
    if (headerEnd == std::string::npos || dataStart == std::string::npos ||
        dataEnd == std::string::npos || layerEnd == std::string::npos) {
      break;
    }

    const auto header = document.substr(layerPosition,
                                       headerEnd - layerPosition + 1);
    const auto layerName = Attribute(header, "name");
    const bool collisionLayer = layerName == "Walls_grass" || layerName == "House";
    const int width = IntegerAttribute(header, "width");
    const int height = IntegerAttribute(header, "height");
    const auto dataTag = std::string("<data encoding=\"csv\">");
    const auto dataBody = document.substr(
        dataStart + dataTag.size(), dataEnd - dataStart - dataTag.size());

    auto emitTiles = [&](const std::vector<unsigned int>& values,
                         int baseX, int baseY, int chunkWidth,
                         int chunkHeight) {
      for (int y = 0; y < chunkHeight; ++y) {
        for (int x = 0; x < chunkWidth; ++x) {
          const auto index = static_cast<std::size_t>(y * chunkWidth + x);
          if (index >= values.size() || values[index] == 0) {
            continue;
          }

          const unsigned int gid = values[index];
          const Tileset* selected = nullptr;
          for (const auto& tileset : tilesets) {
            if (gid >= static_cast<unsigned int>(tileset.firstGid) &&
                (!selected || tileset.firstGid > selected->firstGid)) {
              selected = &tileset;
            }
          }
          if (!selected || !selected->texture ||
              selected->texture->GetWidth() == 0) {
            continue;
          }

          const int localTile = static_cast<int>(gid) - selected->firstGid;
          const int tileColumn = localTile % selected->columns;
          const int tileRow = localTile / selected->columns;
          const glm::vec2 uvScale{
              static_cast<float>(selected->tileWidth) /
                  selected->texture->GetWidth(),
              static_cast<float>(selected->tileHeight) /
                  selected->texture->GetHeight()};

          const auto entity = world.CreateEntity();
          world.Add<components::Transform>(
              entity, components::Transform{
                          {(baseX + x) * selected->tileWidth +
                               selected->tileWidth * 0.5f,
                           (baseY + y) * selected->tileHeight +
                               selected->tileHeight * 0.5f},
                          {0.0f, 0.0f},
                          0.0f,
                          {static_cast<float>(selected->tileWidth),
                           static_cast<float>(selected->tileHeight)}});
          auto& sprite = world.Add<components::SpriteRenderer>(
              entity, components::SpriteRenderer{mesh, shader, selected->texture});
          sprite.layer = layerIndex;
          sprite.uvScale = uvScale;
          sprite.uvOffset = {tileColumn * uvScale.x, tileRow * uvScale.y};
          if (collisionLayer) {
            world.Add<components::Collider>(
                entity,
                components::Collider{{static_cast<float>(selected->tileWidth),
                                      static_cast<float>(selected->tileHeight)},
                                     {0.0f, 0.0f}, true});
          }
        }
      }
    };

    const auto firstChunk = dataBody.find("<chunk");
    if (firstChunk == std::string::npos) {
      emitTiles(ParseCsv(dataBody), 0, 0, width, height);
    } else {
      std::size_t chunkPosition = firstChunk;
      while (chunkPosition != std::string::npos) {
        const auto chunkHeaderEnd = dataBody.find('>', chunkPosition);
        const auto chunkEnd = dataBody.find("</chunk>", chunkHeaderEnd);
        if (chunkHeaderEnd == std::string::npos || chunkEnd == std::string::npos) {
          break;
        }
        const auto chunkHeader = dataBody.substr(
            chunkPosition, chunkHeaderEnd - chunkPosition + 1);
        emitTiles(
            ParseCsv(dataBody.substr(chunkHeaderEnd + 1,
                                     chunkEnd - chunkHeaderEnd - 1)),
            IntegerAttribute(chunkHeader, "x"),
            IntegerAttribute(chunkHeader, "y"),
            IntegerAttribute(chunkHeader, "width"),
            IntegerAttribute(chunkHeader, "height"));
        chunkPosition = dataBody.find("<chunk", chunkEnd + 8);
      }
    }

    ++layerIndex;
    layerPosition = layerEnd + 8;
  }

  std::size_t objectGroupPosition = 0;
  while ((objectGroupPosition = document.find("<objectgroup", objectGroupPosition)) !=
         std::string::npos) {
    const auto groupHeaderEnd = document.find('>', objectGroupPosition);
    const auto groupEnd = document.find("</objectgroup>", groupHeaderEnd);
    if (groupHeaderEnd == std::string::npos || groupEnd == std::string::npos) {
      break;
    }

    std::size_t objectPosition = groupHeaderEnd;
    while ((objectPosition = document.find("<object", objectPosition)) !=
           std::string::npos && objectPosition < groupEnd) {
      const auto objectEnd = document.find('>', objectPosition);
      if (objectEnd == std::string::npos || objectEnd > groupEnd) {
        break;
      }

      const auto object = document.substr(
          objectPosition, objectEnd - objectPosition + 1);
      const float x = std::stof(Attribute(object, "x").empty()
                                     ? "0"
                                     : Attribute(object, "x"));
      const float y = std::stof(Attribute(object, "y").empty()
                                     ? "0"
                                     : Attribute(object, "y"));
      const float width = std::stof(Attribute(object, "width").empty()
                                        ? "0"
                                        : Attribute(object, "width"));
      const float height = std::stof(Attribute(object, "height").empty()
                                         ? "0"
                                         : Attribute(object, "height"));

      if (width > 0.0f && height > 0.0f) {
        const auto entity = world.CreateEntity();
        world.Add<components::Transform>(
            entity, components::Transform{{x + width * 0.5f,
                                           y + height * 0.5f}});
        world.Add<components::Collider>(
            entity, components::Collider{{width, height}, {0.0f, 0.0f}, true});
      }
      objectPosition = objectEnd + 1;
    }
    objectGroupPosition = groupEnd + 14;
  }

  ECLIPSE_INFO("Loaded Tiled map '{}' with {} layers", mapPath, layerIndex);
  return layerIndex > 0;
}

} // namespace eclipse::managers
