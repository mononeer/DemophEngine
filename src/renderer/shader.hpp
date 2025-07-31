
#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Demoph {

class Shader {
public:
    Shader();
    ~Shader();
    
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    void Use();
    
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
    uint32_t GetID() const { return m_programID; }
    
private:
    uint32_t CompileShader(const std::string& source, uint32_t type);
    int GetUniformLocation(const std::string& name);
    
    uint32_t m_programID;
    std::unordered_map<std::string, int> m_uniformLocations;
};

} // namespace Demoph
