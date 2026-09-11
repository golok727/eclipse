#pragma once
#include <cstdint>
#include <string>
namespace eclipse::graphics{
  enum class TextureFilter{
    Nearest,
    Linear
  };

  class Texture{
    public:
      Texture(const std::string& path);
      ~Texture();
      inline const std::string& GetPath(){return mPath;}
      inline uint32_t GetWidth(){return mWidth;}
      inline uint32_t GetHeight(){return mHeight;}
      inline uint32_t GetId(){return mId;}
      inline uint32_t GetNumChannels(){return mNumChannels;}
      inline TextureFilter GetTextureFilter(){return mFilter;}
      void Bind();
      void UnBind();
      void SetTextureFilter(TextureFilter filter);
    private:
      void LoadTexture();
      std::string mPath;
      TextureFilter mFilter;
      uint32_t mId;
      uint32_t mWidth, mHeight;
      uint32_t mNumChannels;
      unsigned char* mPixels;
  };
}
