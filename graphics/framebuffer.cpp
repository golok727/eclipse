#include "framebuffer.h"

#include "../src/log.h"

#include <glad/glad.h>

namespace {

bool CreateFramebuffer(std::uint32_t width, std::uint32_t height,
                       std::uint32_t& framebuffer, std::uint32_t& texture,
                       std::uint32_t& renderbuffer) {
  if (width == 0 || height == 0) {
    return false;
  }

  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width),
               static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture, 0);

  glGenRenderbuffers(1, &renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        static_cast<GLsizei>(width),
                        static_cast<GLsizei>(height));
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, renderbuffer);

  const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (status == GL_FRAMEBUFFER_COMPLETE) {
    return true;
  }

  ECLIPSE_ERROR("Error creating framebuffer: {}", status);
  glDeleteRenderbuffers(1, &renderbuffer);
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &framebuffer);
  framebuffer = 0;
  texture = 0;
  renderbuffer = 0;
  return false;
}

} // namespace

namespace eclipse::graphics {

FrameBuffer::FrameBuffer(std::uint32_t width, std::uint32_t height)
    : mSize(static_cast<int>(width), static_cast<int>(height)),
      mClearColor(1.0f) {
  CreateFramebuffer(width, height, mFbo, mTextureId, mRenderBufferId);
}

FrameBuffer::~FrameBuffer() {
  if (mTextureId != 0) {
    glDeleteTextures(1, &mTextureId);
  }
  if (mRenderBufferId != 0) {
    glDeleteRenderbuffers(1, &mRenderBufferId);
  }
  if (mFbo != 0) {
    glDeleteFramebuffers(1, &mFbo);
  }
}

void FrameBuffer::Resize(std::uint32_t width, std::uint32_t height) {
  if (width == 0 || height == 0 ||
      (mSize.x == static_cast<int>(width) &&
       mSize.y == static_cast<int>(height))) {
    return;
  }

  std::uint32_t framebuffer = 0;
  std::uint32_t texture = 0;
  std::uint32_t renderbuffer = 0;
  if (!CreateFramebuffer(width, height, framebuffer, texture, renderbuffer)) {
    return;
  }

  glDeleteTextures(1, &mTextureId);
  glDeleteRenderbuffers(1, &mRenderBufferId);
  glDeleteFramebuffers(1, &mFbo);
  mFbo = framebuffer;
  mTextureId = texture;
  mRenderBufferId = renderbuffer;
  mSize = {static_cast<int>(width), static_cast<int>(height)};
}

void FrameBuffer::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, mFbo); }

void FrameBuffer::UnBind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

} // namespace eclipse::graphics
