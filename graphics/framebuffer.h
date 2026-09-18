#pragma once
#include <glm/glm.hpp>
#include <cstdint>
namespace eclipse::graphics{
  class FrameBuffer{
    public:
      FrameBuffer(uint32_t width, uint32_t height);
      ~FrameBuffer();
      FrameBuffer(const FrameBuffer&) = delete;
      FrameBuffer& operator=(const FrameBuffer&) = delete;
      FrameBuffer(FrameBuffer&&) = delete;
      FrameBuffer& operator=(FrameBuffer&&) = delete;
      void Resize(uint32_t width, uint32_t height);
      void Bind();
      void UnBind();
      inline const glm::ivec2& GetSize(){return mSize;}
      inline uint32_t GetFbo()const {return mFbo;}
      inline uint32_t GetTextureId() const {return mTextureId;}
      inline uint32_t GetRenderBuffer() const {return mRenderBufferId;}
      inline bool IsValid() const {return mFbo != 0;}
      inline void SetClearColor(const glm::vec4& clearColor){mClearColor = clearColor;}
      inline const glm::vec4& GetClearColor()const {return mClearColor;}
    private:
      uint32_t mFbo = 0;
      glm::ivec2 mSize{0};
      uint32_t mTextureId = 0;
      uint32_t mRenderBufferId = 0;
      glm::vec4 mClearColor{1.0f};
  };
}
