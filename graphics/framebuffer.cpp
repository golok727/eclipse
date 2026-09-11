#include "framebuffer.h"
#include "../src/log.h"
#include <cstdint>
#include <glad/glad.h>
namespace eclipse::graphics{
  FrameBuffer::FrameBuffer(uint32_t width, uint32_t height )
  :mSize(width,height),mRenderBufferId(0),mTextureId(0),mFbo(0),mClearColor(1.f)  {
    glGenFramebuffers(1,&mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER,mFbo);

    glGenTextures(1,&mTextureId);
    glBindTexture(GL_TEXTURE_2D,mTextureId);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,mSize.x,mSize.y,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,mTextureId,0);
    glGenRenderbuffers(1,&mRenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER,mRenderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,mSize.x,mSize.y);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,mRenderBufferId);

    int32_t compileStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(compileStatus != GL_FRAMEBUFFER_COMPLETE){
      ECLIPSE_ERROR("Error creating framebuffer {}",compileStatus);
    }else{
      glBindFramebuffer(GL_FRAMEBUFFER,0);
    }
  }

  FrameBuffer::~FrameBuffer(){
    glDeleteTextures(1, &mTextureId);
    glDeleteRenderbuffers(1, &mRenderBufferId);
    glDeleteFramebuffers(1,&mFbo);
    mFbo = 0;
    mTextureId = 0;
    mRenderBufferId = 0;
  }

  void FrameBuffer::Resize(uint32_t width, uint32_t height){
    if(mSize.x == static_cast<int>(width) && mSize.y == static_cast<int>(height)){
      return;
    }

    mSize = glm::ivec2(width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glDeleteTextures(1, &mTextureId);
    glDeleteRenderbuffers(1, &mRenderBufferId);

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSize.x, mSize.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           mTextureId, 0);

    glGenRenderbuffers(1, &mRenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mSize.x,
                          mSize.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, mRenderBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
}
