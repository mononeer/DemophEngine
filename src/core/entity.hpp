
#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>

namespace Demoph {

class Component {
public:
    virtual ~Component() = default;
    bool enabled = true;
};

class Entity {
public:
    Entity(const std::string& name = "Entity");
    ~Entity() = default;
    
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);
    
    template<typename T>
    T* GetComponent();
    
    template<typename T>
    bool HasComponent() const;
    
    template<typename T>
    void RemoveComponent();
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    bool IsActive() const { return m_active; }
    void SetActive(bool active) { m_active = active; }
    
    uint32_t GetID() const { return m_id; }
    
private:
    static uint32_t s_nextID;
    
    uint32_t m_id;
    std::string m_name;
    bool m_active;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
};

template<typename T, typename... Args>
T* Entity::AddComponent(Args&&... args) {
    static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
    
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    m_components[std::type_index(typeid(T))] = std::move(component);
    return ptr;
}

template<typename T>
T* Entity::GetComponent() {
    auto it = m_components.find(std::type_index(typeid(T)));
    if (it != m_components.end()) {
        return static_cast<T*>(it->second.get());
    }
    return nullptr;
}

template<typename T>
bool Entity::HasComponent() const {
    return m_components.find(std::type_index(typeid(T))) != m_components.end();
}

template<typename T>
void Entity::RemoveComponent() {
    m_components.erase(std::type_index(typeid(T)));
}

} // namespace Demoph
