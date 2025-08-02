
#include "demoph/core/engine.h"
#include "demoph/core/application.h"
#include "demoph/core/scene.h"
#include "demoph/core/layer.h"
#include "demoph/core/log.h"
#include "demoph/renderer/renderer.h"
#include "demoph/physics/physics_world.h"
#include "demoph/audio/audio_engine.h"
#include "demoph/assets/asset_manager.h"
#include "demoph/scripting/script_engine.h"
#include "demoph/platform/window.h"
#include "demoph/input/input.h"

#include <chrono>
#include <thread>
#include <iostream>

namespace Demoph {

Engine* Engine::s_instance = nullptr;

std::unique_ptr<Engine> Engine::Create() {
    if (s_instance) {
        DEMOPH_LOG_ERROR("Engine instance already exists!");
        return nullptr;
    }
    
    return std::make_unique<Engine>();
}

Engine::Engine() {
    if (s_instance) {
        DEMOPH_LOG_ERROR("Multiple engine instances not allowed!");
        return;
    }
    
    s_instance = this;
    m_state = EngineState::Uninitialized;
    
    DEMOPH_LOG_INFO("Demoph Engine v{} - {}", Version::String, Version::Tagline);
    DEMOPH_LOG_INFO("Platform: {}, Configuration: {}, Compiler: {}", 
                    BuildInfo::Platform, BuildInfo::Configuration, BuildInfo::Compiler);
}

Engine::~Engine() {
    if (m_state != EngineState::Shutdown) {
        Shutdown();
    }
    
    s_instance = nullptr;
}

bool Engine::Initialize() {
    return Initialize(EngineConfig{});
}

bool Engine::Initialize(const EngineConfig& config) {
    if (m_state != EngineState::Uninitialized) {
        DEMOPH_LOG_ERROR("Engine already initialized!");
        return false;
    }
    
    m_state = EngineState::Initializing;
    m_config = config;
    
    DEMOPH_LOG_INFO("Initializing Demoph Engine...");
    
    // Initialize logging system
    if (config.enableLogging) {
        Log::Initialize();
        DEMOPH_LOG_INFO("Logging system initialized");
    }
    
    // Initialize core subsystems
    if (!InitializeSubsystems()) {
        DEMOPH_LOG_ERROR("Failed to initialize engine subsystems");
        m_state = EngineState::Uninitialized;
        return false;
    }
    
    m_state = EngineState::Running;
    m_lastFrameTime = std::chrono::duration<float>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
    
    DEMOPH_LOG_INFO("Engine initialized successfully");
    return true;
}

bool Engine::InitializeSubsystems() {
    // Initialize window
    DEMOPH_LOG_INFO("Initializing window system...");
    m_window = Window::Create({
        .title = m_config.windowTitle,
        .width = m_config.windowWidth,
        .height = m_config.windowHeight,
        .fullscreen = m_config.windowFullscreen,
        .vSync = m_config.windowVSync,
        .resizable = m_config.windowResizable
    });
    
    if (!m_window || !m_window->Initialize()) {
        DEMOPH_LOG_ERROR("Failed to initialize window");
        return false;
    }
    
    // Initialize input system
    DEMOPH_LOG_INFO("Initializing input system...");
    if (!Input::Initialize(m_window.get())) {
        DEMOPH_LOG_ERROR("Failed to initialize input system");
        return false;
    }
    
    // Initialize renderer
    DEMOPH_LOG_INFO("Initializing rendering system...");
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->Initialize(m_window.get(), m_config.graphicsAPI)) {
        DEMOPH_LOG_ERROR("Failed to initialize renderer");
        return false;
    }
    
    // Initialize physics
    if (m_config.enablePhysics) {
        DEMOPH_LOG_INFO("Initializing physics system...");
        m_physics = std::make_unique<PhysicsWorld>();
        if (!m_physics->Initialize()) {
            DEMOPH_LOG_ERROR("Failed to initialize physics");
            return false;
        }
    }
    
    // Initialize audio
    if (m_config.enableAudio) {
        DEMOPH_LOG_INFO("Initializing audio system...");
        m_audio = std::make_unique<AudioEngine>();
        if (!m_audio->Initialize()) {
            DEMOPH_LOG_ERROR("Failed to initialize audio");
            return false;
        }
        m_audio->SetMasterVolume(m_config.masterVolume);
    }
    
    // Initialize asset manager
    DEMOPH_LOG_INFO("Initializing asset management system...");
    m_assets = std::make_unique<AssetManager>();
    if (!m_assets->Initialize(m_config.assetsPath)) {
        DEMOPH_LOG_ERROR("Failed to initialize asset manager");
        return false;
    }
    
    // Initialize scripting
    if (m_config.enableScripting) {
        DEMOPH_LOG_INFO("Initializing scripting system...");
        m_scripting = std::make_unique<ScriptEngine>();
        if (!m_scripting->Initialize(m_config.scriptingLanguage)) {
            DEMOPH_LOG_ERROR("Failed to initialize scripting");
            return false;
        }
    }
    
    DEMOPH_LOG_INFO("All subsystems initialized successfully");
    return true;
}

void Engine::Run() {
    if (m_state != EngineState::Running) {
        DEMOPH_LOG_ERROR("Engine not in running state");
        return;
    }
    
    DEMOPH_LOG_INFO("Starting main engine loop");
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (ShouldRun()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        m_deltaTime = deltaTime;
        m_time += deltaTime;
        m_frameCount++;
        
        ProcessEvents();
        Update(deltaTime);
        Render();
        UpdateFPS();
        
        // Handle window close event
        if (m_window && m_window->ShouldClose()) {
            RequestShutdown();
        }
    }
    
    DEMOPH_LOG_INFO("Main engine loop ended");
}

void Engine::ProcessEvents() {
    if (m_window) {
        m_window->PollEvents();
    }
    
    Input::Update();
}

void Engine::Update(float deltaTime) {
    // Update application
    if (m_application) {
        m_application->Update(deltaTime);
    }
    
    // Update layers
    for (auto& layer : m_layers) {
        if (layer->IsEnabled()) {
            layer->Update(deltaTime);
        }
    }
    
    // Update active scene
    if (m_activeScene) {
        m_activeScene->Update(deltaTime);
    }
    
    // Update physics
    if (m_physics) {
        m_physics->Update(deltaTime);
    }
    
    // Update audio
    if (m_audio) {
        m_audio->Update(deltaTime);
    }
    
    // Update scripting
    if (m_scripting) {
        m_scripting->Update(deltaTime);
    }
    
    // Call update callbacks
    for (const auto& callback : m_updateCallbacks) {
        callback();
    }
}

void Engine::Render() {
    if (!m_renderer) {
        return;
    }
    
    m_renderer->BeginFrame();
    
    // Render application
    if (m_application) {
        m_application->Render();
    }
    
    // Render layers
    for (auto& layer : m_layers) {
        if (layer->IsEnabled()) {
            layer->Render();
        }
    }
    
    // Render active scene
    if (m_activeScene) {
        m_activeScene->Render(m_renderer.get());
    }
    
    // Render overlays
    for (auto& overlay : m_overlays) {
        if (overlay->IsEnabled()) {
            overlay->Render();
        }
    }
    
    // Call render callbacks
    for (const auto& callback : m_renderCallbacks) {
        callback();
    }
    
    m_renderer->EndFrame();
    
    if (m_window) {
        m_window->SwapBuffers();
    }
}

void Engine::UpdateFPS() {
    m_fpsUpdateTimer += m_deltaTime;
    
    if (m_fpsUpdateTimer >= 1.0f) {
        m_fps = m_frameCount / m_fpsUpdateTimer;
        m_fpsUpdateTimer = 0.0f;
        static uint64_t lastFrameCount = 0;
        lastFrameCount = m_frameCount;
    }
}

void Engine::Shutdown() {
    if (m_state == EngineState::Shutdown) {
        return;
    }
    
    DEMOPH_LOG_INFO("Shutting down Demoph Engine...");
    m_state = EngineState::Shutting_Down;
    
    // Call shutdown callbacks
    for (const auto& callback : m_shutdownCallbacks) {
        callback();
    }
    
    // Clear layers
    m_overlays.clear();
    m_layers.clear();
    
    // Clear scenes
    m_activeScene = nullptr;
    m_scenes.clear();
    
    // Clear application
    m_application.reset();
    
    // Shutdown subsystems
    ShutdownSubsystems();
    
    m_state = EngineState::Shutdown;
    DEMOPH_LOG_INFO("Engine shutdown complete");
}

void Engine::ShutdownSubsystems() {
    if (m_scripting) {
        m_scripting->Shutdown();
        m_scripting.reset();
    }
    
    if (m_assets) {
        m_assets->Shutdown();
        m_assets.reset();
    }
    
    if (m_audio) {
        m_audio->Shutdown();
        m_audio.reset();
    }
    
    if (m_physics) {
        m_physics->Shutdown();
        m_physics.reset();
    }
    
    if (m_renderer) {
        m_renderer->Shutdown();
        m_renderer.reset();
    }
    
    Input::Shutdown();
    
    if (m_window) {
        m_window->Shutdown();
        m_window.reset();
    }
}

void Engine::SetApplication(std::unique_ptr<Application> app) {
    m_application = std::move(app);
    if (m_application) {
        m_application->Initialize();
    }
}

Scene* Engine::CreateScene(const std::string& name) {
    auto scene = std::make_unique<Scene>(name);
    Scene* scenePtr = scene.get();
    m_scenes.push_back(std::move(scene));
    
    if (!m_activeScene) {
        m_activeScene = scenePtr;
    }
    
    DEMOPH_LOG_INFO("Created scene: {}", name);
    return scenePtr;
}

void Engine::DestroyScene(Scene* scene) {
    if (scene == m_activeScene) {
        m_activeScene = nullptr;
    }
    
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [scene](const std::unique_ptr<Scene>& s) { return s.get() == scene; });
    
    if (it != m_scenes.end()) {
        DEMOPH_LOG_INFO("Destroyed scene: {}", (*it)->GetName());
        m_scenes.erase(it);
    }
}

void Engine::SetActiveScene(Scene* scene) {
    if (std::find_if(m_scenes.begin(), m_scenes.end(),
        [scene](const std::unique_ptr<Scene>& s) { return s.get() == scene; }) != m_scenes.end()) {
        m_activeScene = scene;
        DEMOPH_LOG_INFO("Set active scene: {}", scene->GetName());
    }
}

void Engine::PushLayer(std::unique_ptr<Layer> layer) {
    layer->OnAttach();
    m_layers.push_back(std::move(layer));
}

void Engine::PushOverlay(std::unique_ptr<Layer> overlay) {
    overlay->OnAttach();
    m_overlays.push_back(std::move(overlay));
}

void Engine::PopLayer() {
    if (!m_layers.empty()) {
        m_layers.back()->OnDetach();
        m_layers.pop_back();
    }
}

void Engine::PopOverlay() {
    if (!m_overlays.empty()) {
        m_overlays.back()->OnDetach();
        m_overlays.pop_back();
    }
}

} // namespace Demoph
