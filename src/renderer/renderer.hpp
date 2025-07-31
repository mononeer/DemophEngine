
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Demoph {

class Entity;
class Shader;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool Initialize();
    void Shutdown();
    
    void BeginFrame();
    void EndFrame();
    
    void RenderEntity(std::shared_ptr<Entity> entity);
    
    void SetViewMatrix(const glm::mat4& view) { m_viewMatrix = view; }
    void SetProjectionMatrix(const glm::mat4& projection) { m_projectionMatrix = projection; }
    
private:
    bool InitializeGL();
    void LoadDefaultShaders();
    
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
    
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    
    // Render stats
    uint32_t m_drawCalls;
    uint32_t m_verticesRendered;
};

} // namespace Demoph
