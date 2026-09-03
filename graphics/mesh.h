#pragma once

#include <cstdint>
namespace eclipse::graphics{
  class Mesh{
    public:
      Mesh(float* vertexArray, uint32_t vertexCount , uint32_t dimensions);

      Mesh(float* vertexArray, uint32_t vertexCount , uint32_t dimensions,uint32_t* elementArray, uint32_t elementCount);
      ~Mesh();
      void Bind();
      void Unbind();
      inline uint32_t GetElementCount(){return mElementCount;}
      inline uint32_t GetVertexCount(){return mVertexCount;}
    private:
      uint32_t mVao,mEbo;
      uint32_t mVertexCount,mElementCount;
      uint32_t mPositionVbo;
  };
}
