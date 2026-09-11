#include "shader.h"
#include "helpers.h"
#include <cstdint>
#include <glad/glad.h>
#include "../glm/gtc/type_ptr.hpp"
namespace eclipse::graphics {

Shader::Shader(const std::string& vertex, const std::string& fragment){
  mProgramId = glCreateProgram(); ECLIPSE_CHECK_GL_ERROR;
  char errorLog[512];
  int status = GL_FALSE;

    uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);ECLIPSE_CHECK_GL_ERROR;

  {
    const GLchar* glSource = vertex.c_str();ECLIPSE_CHECK_GL_ERROR;
    glShaderSource(vertexShaderId,1,&glSource,0);ECLIPSE_CHECK_GL_ERROR;
    glCompileShader(vertexShaderId);ECLIPSE_CHECK_GL_ERROR;
    glGetShaderiv(vertexShaderId,GL_COMPILE_STATUS,&status);ECLIPSE_CHECK_GL_ERROR;
    if(status != GL_TRUE){
      glGetShaderInfoLog(vertexShaderId,sizeof(errorLog),0,errorLog);ECLIPSE_CHECK_GL_ERROR;
      ECLIPSE_ERROR("Vertex Shader Compilation Error {}",errorLog);
    }else{
      glAttachShader(mProgramId,vertexShaderId);ECLIPSE_CHECK_GL_ERROR;
    }
  }

    uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);ECLIPSE_CHECK_GL_ERROR;

if(status == GL_TRUE)
  
  {
    const GLchar* glSource = fragment.c_str();ECLIPSE_CHECK_GL_ERROR;
    glShaderSource(fragmentShaderId,1,&glSource,0);ECLIPSE_CHECK_GL_ERROR;
    glCompileShader(fragmentShaderId);ECLIPSE_CHECK_GL_ERROR;
    glGetShaderiv(fragmentShaderId,GL_COMPILE_STATUS,&status);ECLIPSE_CHECK_GL_ERROR;
    if(status != GL_TRUE){
      glGetShaderInfoLog(fragmentShaderId,sizeof(errorLog),0,errorLog);ECLIPSE_CHECK_GL_ERROR;
      ECLIPSE_ERROR("fragment Shader Compilation Error {}",errorLog);
    }else{
      glAttachShader(mProgramId,fragmentShaderId);ECLIPSE_CHECK_GL_ERROR;
    }
  }
  
if(status == GL_TRUE)

{
  glLinkProgram(mProgramId);ECLIPSE_CHECK_GL_ERROR;
  glValidateProgram(mProgramId);ECLIPSE_CHECK_GL_ERROR;
  glGetProgramiv(mProgramId,GL_LINK_STATUS,&status);ECLIPSE_CHECK_GL_ERROR;
  if(status != GL_TRUE){
    glGetProgramInfoLog(mProgramId,sizeof(errorLog),0,errorLog);ECLIPSE_CHECK_GL_ERROR;
    ECLIPSE_ERROR("Program linking error",errorLog)
    mProgramId = -1;
  }
}

glDeleteShader(vertexShaderId);
glDeleteShader(fragmentShaderId);
  
}
Shader::~Shader(){
  glUseProgram(0);
  glDeleteProgram(mProgramId);  
}
void Shader::Bind(){
  glUseProgram(mProgramId);
}
void Shader::Unbind(){
  glUseProgram(0);
}
void Shader::SetUniformInt(const std::string &name, int val){
  glUseProgram(mProgramId);
  glUniform1i(GetUniformLocations(name),val);
}
void Shader::SetUniformFloat(const std::string &name, float val){
  glUseProgram(mProgramId);
  glUniform1f(GetUniformLocations(name),val);
}
void Shader::SetUniformFloat2(const std::string &name, float val1, float val2){
    glUseProgram(mProgramId);
  glUniform2f(GetUniformLocations(name),val1, val2);
}
void Shader::SetUniformFloat3(const std::string &name, float val1, float val2, float val3){
  glUseProgram(mProgramId);
  glUniform3f(GetUniformLocations(name),val1, val2, val3);
}
void Shader::SetUniformFloat4(const std::string &name, float val1, float val2, float val3, float val4){
  glUseProgram(mProgramId);
  glUniform4f(GetUniformLocations(name),val1, val2, val3,val4);
}

void Shader::SetUniformFloat2(const std::string& name, const glm::vec2& val){
    SetUniformFloat2(name,val.x,val.y);ECLIPSE_CHECK_GL_ERROR;
  }
  void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& val){

    SetUniformFloat3(name,val.x,val.y,val.z);ECLIPSE_CHECK_GL_ERROR;
  }
  void Shader::SetUniformFloat4(const std::string& name, const glm::vec4& val){

    SetUniformFloat4(name,val.x,val.y,val.z,val.w);ECLIPSE_CHECK_GL_ERROR;
  }

  void Shader::SetUniformMat3(const std::string& name , const glm::mat3& mat){
    glUseProgram(mProgramId);ECLIPSE_CHECK_GL_ERROR;
    glUniformMatrix3fv(GetUniformLocations(name),1,GL_FALSE,glm::value_ptr(mat));ECLIPSE_CHECK_GL_ERROR;
  }
  void Shader::SetUniformMat4(const std::string& name , const glm::mat4& mat){
    glUseProgram(mProgramId);ECLIPSE_CHECK_GL_ERROR;
    glUniformMatrix4fv(GetUniformLocations(name),1,GL_FALSE,glm::value_ptr(mat));ECLIPSE_CHECK_GL_ERROR;

  }

int Shader::GetUniformLocations(const std::string &name){
  auto iterator = mGetUniformLocations.find(name);
  if(iterator == mGetUniformLocations.end()){
    mGetUniformLocations[name] = glGetUniformLocation(mProgramId,name.c_str());
  }
  return mGetUniformLocations[name];
}
} // namespace eclipse::graphics
