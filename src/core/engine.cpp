
#include "engine.hpp"
#include "entity.hpp"
#include "system.hpp"
#include "renderer/renderer.hpp"
#include "physics/physics_engine.hpp"
#include "audio/audio_engine.hpp"
#include "assets/asset_manager.hpp"
#include <iostream>
#include <thread>

namespace Demoph {

Engine::Engine() 
    : m_state(EngineState::INITIALIZING)
    , m_deltaTime(0.0f)
    , m_frameCount(0)
    , m_running(false) {
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize() {
    std::cout << "Initializing Demoph Engine - Where Precision Meets Possibility\n";
    
    // Initialize core systems
    m_renderer = std::make_unique<Renderer>();
    m_physics = std::make_unique<PhysicsEngine>();
    m_audio = std::make_unique<AudioEngine>();
    m_assetManager = std::make_unique<AssetManager>();
    
    if (!m_renderer->Initialize()) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }
    
    if (!m_physics->Initialize()) {
        std::cerr << "Failed to initialize physics\n";
        return false;
    }
    
    if (!m_audio->Initialize()) {
        std::cerr << "Failed to initialize audio\n";
        return false;
    }
    
    if (!m_assetManager->Initialize()) {
        std::cerr << "Failed to initialize asset manager\n";
        return false;
    }
    
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_state = EngineState::RUNNING;
    m_running = true;
    
    std::cout << "Engine initialized successfully\n";
    return true;
}

void Engine::Run() {
    while (m_running && m_state == EngineState::RUNNING) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastFrameTime);
        m_deltaTime = duration.count() / 1000000.0f;
        m_lastFrameTime = currentTime;
        
        Update(m_deltaTime);
        Render();
        
        m_frameCount++;
        
        // Cap framerate to 60 FPS
        std::this_thread::sleep_for(std::chrono::microseconds(16667));
    }
}

void Engine::Update(float deltaTime) {
    // Update physics
    m_physics->Update(deltaTime);
    
    // Update all systems
    for (auto& system : m_systems) {
        system->Update(m_entities, deltaTime);
    }
    
    // Update audio
    m_audio->Update(deltaTime);
}

void Engine::Render() {
    m_renderer->BeginFrame();
    
    // Render all entities
    for (auto& entity : m_entities) {
        m_renderer->RenderEntity(entity);
    }
    
    m_renderer->EndFrame();
}

void Engine::Shutdown() {
    std::cout << "Shutting down Demoph Engine\n";
    m_state = EngineState::SHUTTING_DOWN;
    m_running = false;
    
    m_entities.clear();
    m_systems.clear();
    
    if (m_audio) m_audio->Shutdown();
    if (m_physics) m_physics->Shutdown();
    if (m_renderer) m_renderer->Shutdown();
    if (m_assetManager) m_assetManager->Shutdown();
    
    m_state = EngineState::STOPPED;
    std::cout << "Engine shutdown complete\n";
}

std::shared_ptr<Entity> Engine::CreateEntity(const std::string& name) {
    auto entity = std::make_shared<Entity>(name);
    m_entities.push_back(entity);
    return entity;
}

void Engine::DestroyEntity(std::shared_ptr<Entity> entity) {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
}

} // namespace Demoph
