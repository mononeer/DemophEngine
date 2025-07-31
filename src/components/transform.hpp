
#pragma once

#include "../core/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Demoph {

class Transform : public Component {
public:
    Transform(const glm::vec3& position = glm::vec3(0.0f),
              const glm::vec3& rotation = glm::vec3(0.0f),
              const glm::vec3& scale = glm::vec3(1.0f))
        : position(position), rotation(rotation), scale(scale) {}
    
    glm::mat4 GetMatrix() const {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
        r = glm::rotate(r, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        r = glm::rotate(r, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
        return t * r * s;
    }
    
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

} // namespace Demoph
