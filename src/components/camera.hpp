
#pragma once

#include "../core/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Demoph {

class Camera : public Component {
public:
    Camera(float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f)
        : fieldOfView(fov), nearPlane(nearPlane), farPlane(farPlane) {}
    
    glm::mat4 GetProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    }
    
    glm::mat4 GetViewMatrix(const glm::vec3& position, const glm::vec3& rotation) const {
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        view = glm::translate(view, -position);
        return view;
    }
    
    float fieldOfView;
    float nearPlane;
    float farPlane;
    bool isActive = true;
};

} // namespace Demoph
