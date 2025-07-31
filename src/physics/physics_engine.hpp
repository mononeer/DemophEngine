
#pragma once

#include <vector>
#include <memory>

namespace Demoph {

class Entity;

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();
    
    bool Initialize();
    void Shutdown();
    
    void Update(float deltaTime);
    
    void SetGravity(float gravity) { m_gravity = gravity; }
    float GetGravity() const { return m_gravity; }
    
private:
    void UpdateRigidBodies(const std::vector<std::shared_ptr<Entity>>& entities, float deltaTime);
    void CheckCollisions();
    
    float m_gravity;
    float m_fixedTimeStep;
    float m_accumulator;
};

} // namespace Demoph
