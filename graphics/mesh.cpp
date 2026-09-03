#include "mesh.h"
#include <cstdint>
#include <glad/glad.h>
#include "helpers.h"
#include "../src/log.h"
namespace eclipse::graphics {
Mesh::Mesh(float *vertexArray, uint32_t vertexCount, uint32_t dimensions)
:mVertexCount(vertexCount),mEbo(0),mElementCount(0)

{
  glGenVertexArrays(1,&mVao);ECLIPSE_CHECK_GL_ERROR;
  glBindVertexArray(mVao);ECLIPSE_CHECK_GL_ERROR;
  glGenBuffers(1,&mPositionVbo);ECLIPSE_CHECK_GL_ERROR;
  glBindBuffer(GL_ARRAY_BUFFER,mPositionVbo);ECLIPSE_CHECK_GL_ERROR;
  glBufferData(GL_ARRAY_BUFFER,sizeof(float)* dimensions* vertexCount,vertexArray,GL_STATIC_DRAW);ECLIPSE_CHECK_GL_ERROR;
  glEnableVertexAttribArray(0);ECLIPSE_CHECK_GL_ERROR;
  glVertexAttribPointer(0,dimensions,GL_FLOAT,GL_FALSE,0,0);ECLIPSE_CHECK_GL_ERROR;
  glDisableVertexAttribArray(0);ECLIPSE_CHECK_GL_ERROR;
  glBindBuffer(GL_ARRAY_BUFFER,0);ECLIPSE_CHECK_GL_ERROR;
  glBindVertexArray(0);  ECLIPSE_CHECK_GL_ERROR;
}

Mesh::Mesh(float *vertexArray, uint32_t vertexCount, uint32_t dimensions,
           uint32_t *elementArray, uint32_t elementCount)

:Mesh(vertexArray,vertexCount,dimensions)
{
  mElementCount = elementCount;
  // glGenVertexArrays(1,&mVao);ECLIPSE_CHECK_GL_ERROR;
  glBindVertexArray(mVao);ECLIPSE_CHECK_GL_ERROR;
  glGenBuffers(1,&mEbo);ECLIPSE_CHECK_GL_ERROR;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mEbo);ECLIPSE_CHECK_GL_ERROR;
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(uint32_t) * elementCount,elementArray,GL_STATIC_DRAW);ECLIPSE_CHECK_GL_ERROR;
  // glEnableVertexAttribArray(0);ECLIPSE_CHECK_GL_ERROR;
  // glVertexAttribPointer(0,dimensions,GL_FLOAT,GL_FALSE,0,0);ECLIPSE_CHECK_GL_ERROR;
  // glDisableVertexAttribArray(0);ECLIPSE_CHECK_GL_ERROR;
  // glBindBuffer(GL_ARRAY_BUFFER,0);ECLIPSE_CHECK_GL_ERROR;
  glBindVertexArray(0);  ECLIPSE_CHECK_GL_ERROR;
  
}

Mesh::~Mesh(){
  glDeleteBuffers(1,&mPositionVbo);
  if(mEbo != 0){
    glDeleteBuffers(1,&mEbo);
  }
  glDeleteVertexArrays(1,&mVao);
}

void Mesh::Bind(){
  glBindVertexArray(mVao);
  glEnableVertexAttribArray(0);
}

void Mesh::Unbind(){
  glDisableVertexAttribArray(0);
  glBindVertexArray(0); }

} // namespace eclipse::graphics
