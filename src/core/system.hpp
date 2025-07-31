
#pragma once

#include <vector>
#include <memory>

namespace Demoph {

class Entity;

class System {
public:
    virtual ~System() = default;
    
    virtual void Update(const std::vector<std::shared_ptr<Entity>>& entities, float deltaTime) = 0;
    
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    int GetPriority() const { return m_priority; }
    void SetPriority(int priority) { m_priority = priority; }
    
protected:
    bool m_enabled = true;
    int m_priority = 0;
};

} // namespace Demoph
