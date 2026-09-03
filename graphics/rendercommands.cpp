#include "rendercommands.h"
#include <memory>
#include "mesh.h"
#include "shader.h"
#include <glad/glad.h>
#include "../src/log.h"
#include "../src/engine.h"
namespace eclipse::graphics::rendercommands{
  void RenderMesh::Execute(){
    std::shared_ptr<graphics::Mesh>mesh = mMesh.lock();
    std::shared_ptr<graphics::Shader>shader= mShader.lock();
    if(mesh&& shader){
        mesh ->Bind();
        shader ->Bind();
        if(mesh ->GetElementCount() >0){
          glDrawElements(GL_TRIANGLES,mesh ->GetElementCount(),GL_UNSIGNED_INT,0);
        } else{
          glDrawArrays(GL_TRIANGLE_STRIP,0,mesh->GetVertexCount());
        }
        mesh ->Unbind();
        shader ->Unbind();
    }
    else{
      ECLIPSE_WARN("Initializing with invalid shader data");
    }    
  }

  
}
