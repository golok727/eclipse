#include "shader.h"
#include "../src/log.h"
#include "glad/glad.h"
#include "helpers.h"
#include <cstdint>
#include <string>
namespace eclipse::graphics {

Shader::Shader(const std::string &vertex, const std::string &fragment) {
  mProgramId = glCreateProgram();
  char errorLog[512];
  int status = false;

  uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

  {
    const GLchar *glSource = vertex.c_str();
    ECLIPSE_CHECK_GL_ERROR;
    glShaderSource(vertexShaderId, 1, &glSource, 0);
    ECLIPSE_CHECK_GL_ERROR;
    glCompileShader(vertexShaderId);
    ECLIPSE_CHECK_GL_ERROR;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
    ECLIPSE_CHECK_GL_ERROR;
    if (status != GL_TRUE) {
      glGetShaderInfoLog(vertexShaderId, sizeof(errorLog), 0, errorLog);
      ECLIPSE_CHECK_GL_ERROR;
      ECLIPSE_ERROR("Vertex Shader Compilation Error {}", errorLog);
      ECLIPSE_CHECK_GL_ERROR;
    }
    glAttachShader(mProgramId, vertexShaderId);
    ECLIPSE_CHECK_GL_ERROR;
  }

  uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

  if (status == GL_TRUE)

  {
    const GLchar *glSource = fragment.c_str();
    ECLIPSE_CHECK_GL_ERROR;
    glShaderSource(fragmentShaderId, 1, &glSource, 0);
    ECLIPSE_CHECK_GL_ERROR;
    glCompileShader(fragmentShaderId);
    ECLIPSE_CHECK_GL_ERROR;
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &status);
    ECLIPSE_CHECK_GL_ERROR;
    if (status != GL_TRUE) {
      glGetShaderInfoLog(fragmentShaderId, sizeof(errorLog), 0, errorLog);
      ECLIPSE_CHECK_GL_ERROR;
      ECLIPSE_ERROR("fragment Shader Compilation Error {}", errorLog);
      ECLIPSE_CHECK_GL_ERROR;
    }
    glAttachShader(mProgramId, fragmentShaderId);
    ECLIPSE_CHECK_GL_ERROR;
  }

  ECLIPSE_ASSERT(status == GL_TRUE, "Error Compiling Shaders");

  if (status == GL_TRUE) {
    glLinkProgram(mProgramId);
    glValidateProgram(mProgramId);
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
      glGetProgramInfoLog(mProgramId, sizeof(errorLog), 0, errorLog);
      mProgramId = -1;
    }
  }

  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
}
Shader::~Shader() {
  glUseProgram(0);
  glDeleteProgram(mProgramId);
}
void Shader::Bind() { glUseProgram(mProgramId); }
void Shader::Unbind() { glUseProgram(0); }
void Shader::SetUniformInt(const std::string &name, int val) {
  glUseProgram(mProgramId);
  glUniform1i(GetUniformLocation(name), val);
}

void Shader::SetUniformFloat(const std::string &name, float val) {
  glUseProgram(mProgramId);
  glUniform1f(GetUniformLocation(name), val);
}

void Shader::SetUniformFloat2(const std::string &name, float val1, float val2) {
  glUseProgram(mProgramId);
  glUniform2f(GetUniformLocation(name), val1, val2);
}

void Shader::SetUniformFloat3(const std::string &name, float val1, float val2,
                              float val3) {
  glUseProgram(mProgramId);
  glUniform3f(GetUniformLocation(name), val1, val2, val3);
}

void Shader::SetUniformFloat4(const std::string &name, float val1, float val2,
                              float val3, float val4) {
  glUseProgram(mProgramId);
  glUniform4f(GetUniformLocation(name), val1, val2, val3, val4);
}

int Shader::GetUniformLocation(const std::string &name) {
  auto it = mGetUniformLocations.find(name);
  if (it == mGetUniformLocations.end()) {
    mGetUniformLocations[name] = glGetUniformLocation(mProgramId, name.c_str());
  }
  return mGetUniformLocations[name];
}
} // namespace eclipse::graphics
