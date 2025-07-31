
#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

namespace Demoph {

class Entity;
class System;
class Component;
class Renderer;
class PhysicsEngine;
class AudioEngine;
class AssetManager;

enum class EngineState {
    INITIALIZING,
    RUNNING,
    PAUSED,
    SHUTTING_DOWN,
    STOPPED
};

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize();
    void Run();
    void Shutdown();
    
    void Update(float deltaTime);
    void Render();
    
    // Entity management
    std::shared_ptr<Entity> CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(std::shared_ptr<Entity> entity);
    
    // System management
    template<typename T, typename... Args>
    void AddSystem(Args&&... args);
    
    template<typename T>
    T* GetSystem();
    
    // Getters
    EngineState GetState() const { return m_state; }
    float GetDeltaTime() const { return m_deltaTime; }
    uint64_t GetFrameCount() const { return m_frameCount; }
    
private:
    EngineState m_state;
    
    std::vector<std::shared_ptr<Entity>> m_entities;
    std::vector<std::unique_ptr<System>> m_systems;
    
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<PhysicsEngine> m_physics;
    std::unique_ptr<AudioEngine> m_audio;
    std::unique_ptr<AssetManager> m_assetManager;
    
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    float m_deltaTime;
    uint64_t m_frameCount;
    
    bool m_running;
};

} // namespace Demoph
