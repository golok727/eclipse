#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace eclipse::graphics {

class Shader {
public:
  Shader(const std::string& vertex, const std::string& fragment);
  ~Shader();

  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  Shader(Shader&&) = delete;
  Shader& operator=(Shader&&) = delete;

  bool IsValid() const { return mProgramId != 0; }
  void Bind() const;
  void Unbind() const;
  void SetUniformInt(const std::string& name, int value);
  void SetUniformFloat(const std::string& name, float value);
  void SetUniformFloat2(const std::string& name, float value1, float value2);
  void SetUniformFloat3(const std::string& name, float value1, float value2,
                        float value3);
  void SetUniformFloat4(const std::string& name, float value1, float value2,
                        float value3, float value4);
  void SetUniformFloat2(const std::string& name, const glm::vec2& value);
  void SetUniformFloat3(const std::string& name, const glm::vec3& value);
  void SetUniformFloat4(const std::string& name, const glm::vec4& value);
  void SetUniformMat3(const std::string& name, const glm::mat3& matrix);
  void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
  int GetUniformLocation(const std::string& name);

  std::uint32_t mProgramId = 0;
  std::unordered_map<std::string, int> mUniformLocations;
};

} // namespace eclipse::graphics
