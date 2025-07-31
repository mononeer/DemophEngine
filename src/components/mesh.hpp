
#pragma once

#include "../core/entity.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Demoph {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
};

class Mesh : public Component {
public:
    Mesh() = default;
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string materialID;
    
    // GPU handles (set by renderer)
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t EBO = 0;
};

} // namespace Demoph
