#pragma once
#include "../glm/glm.hpp"
#include <cstdint>
namespace eclipse::graphics{
  class FrameBuffer{
    public:
      FrameBuffer(uint32_t width, uint32_t height);
      ~FrameBuffer();
      void Resize(uint32_t width, uint32_t height);
      void Bind();
      void UnBind();
      inline const glm::ivec2& GetSize(){return mSize;}
      inline uint32_t GetFbo()const {return mFbo;}
      inline uint32_t GetTextureId() const {return mTextureId;}
      inline uint32_t GetRenderBuffer() const {return mRenderBufferId;}
      inline void SetClearColor(const glm::vec4& clearColor){mClearColor = clearColor;}
      inline const glm::vec4& GetClearColor()const {return mClearColor;}
    private:
      uint32_t mFbo;
      glm::ivec2 mSize;
      uint32_t mTextureId;
      uint32_t mRenderBufferId;
      glm::vec4 mClearColor;
  };
}
