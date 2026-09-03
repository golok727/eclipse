#include "framebuffer.h"
#include "../src/log.h"
#include <cstdint>
#include <glad/glad.h>
namespace eclipse::graphics {

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
    : mHeight(height), mWidth(width), mFbo(0), mTextureId(0), mRenderBuffId(0),
      mCCR(1), mCCA(1), mCCB(1), mCCG(1)

{

  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

  glGenTextures(1, &mTextureId);
  glBindTexture(GL_TEXTURE_2D, mTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture(GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0, mTextureId, 0);

  glGenRenderbuffers(1, &mRenderBuffId);
  glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERER, mRenderBuffId);

  int32_t compileStatus = glCheckFramebufferStatus(mTextureId);
  if (compileStatus != GL_TRUE) {
    ECLIPSE_ERROR("Framebuffer creation failed:  ")
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer() {
  glDeleteFramebuffers(1,&mFbo);
  mFbo = 0;
  mTextureId = 0;
  mRenderBuffId = 0;
}



} // namespace eclipse::graphics
