
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace Demoph {

class Application;
class Scene;
class Renderer;
class PhysicsWorld;
class AudioEngine;
class AssetManager;
class ScriptEngine;
class Window;
class Layer;

/**
 * @brief Engine initialization parameters
 */
struct EngineConfig {
    std::string windowTitle = "Demoph Engine";
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    bool windowFullscreen = false;
    bool windowVSync = true;
    bool windowResizable = true;
    
    std::string graphicsAPI = "auto"; // "vulkan", "opengl", "auto"
    uint32_t msaaSamples = 4;
    bool enableVSync = true;
    
    bool enablePhysics = true;
    float physicsTimeStep = 1.0f / 60.0f;
    
    bool enableAudio = true;
    float masterVolume = 1.0f;
    
    bool enableScripting = true;
    std::string scriptingLanguage = "python"; // "python", "lua"
    
    std::string assetsPath = "assets/";
    std::string shadersPath = "shaders/";
    std::string configPath = "config/";
    
    bool enableImGui = true;
    bool enableProfiler = true;
    bool enableLogging = true;
};

/**
 * @brief Engine state enumeration
 */
enum class EngineState {
    Uninitialized,
    Initializing,
    Running,
    Paused,
    Shutting_Down,
    Shutdown
};

/**
 * @brief Main engine class
 * 
 * The Engine class is the central hub of the Demoph Engine. It manages all major
 * subsystems including rendering, physics, audio, input, and scripting.
 * 
 * @example
 * auto engine = Demoph::Engine::Create();
 * if (engine->Initialize()) {
 *     engine->Run();
 * }
 */
class DEMOPH_API Engine {
public:
    /**
     * @brief Create a new engine instance
     * @return Unique pointer to the created engine
     */
    static std::unique_ptr<Engine> Create();
    
    /**
     * @brief Get the singleton engine instance
     * @return Pointer to the engine instance, nullptr if not created
     */
    static Engine* Get() { return s_instance; }
    
    Engine();
    virtual ~Engine();
    
    DEMOPH_NO_COPY_NO_MOVE(Engine)
    
    /**
     * @brief Initialize the engine with default configuration
     * @return true if initialization was successful
     */
    bool Initialize();
    
    /**
     * @brief Initialize the engine with custom configuration
     * @param config Engine configuration parameters
     * @return true if initialization was successful
     */
    bool Initialize(const EngineConfig& config);
    
    /**
     * @brief Run the engine main loop
     * 
     * This function will block until the engine is shut down.
     * It handles the main game loop including update and render cycles.
     */
    void Run();
    
    /**
     * @brief Shutdown the engine
     * 
     * Cleanly shuts down all engine subsystems and releases resources.
     */
    void Shutdown();
    
    /**
     * @brief Update the engine for one frame
     * @param deltaTime Time elapsed since last frame in seconds
     */
    void Update(float deltaTime);
    
    /**
     * @brief Render one frame
     */
    void Render();
    
    /**
     * @brief Request engine shutdown
     * 
     * Sets a flag to shut down the engine at the end of the current frame.
     */
    void RequestShutdown() { m_shouldShutdown = true; }
    
    /**
     * @brief Check if engine should continue running
     * @return true if engine should continue running
     */
    bool ShouldRun() const { return m_state == EngineState::Running && !m_shouldShutdown; }
    
    // Subsystem getters
    Renderer* GetRenderer() const { return m_renderer.get(); }
    PhysicsWorld* GetPhysics() const { return m_physics.get(); }
    AudioEngine* GetAudio() const { return m_audio.get(); }
    AssetManager* GetAssets() const { return m_assets.get(); }
    ScriptEngine* GetScripting() const { return m_scripting.get(); }
    Window* GetWindow() const { return m_window.get(); }
    
    // Application management
    void SetApplication(std::unique_ptr<Application> app);
    Application* GetApplication() const { return m_application.get(); }
    
    // Scene management
    Scene* CreateScene(const std::string& name);
    void DestroyScene(Scene* scene);
    void SetActiveScene(Scene* scene);
    Scene* GetActiveScene() const { return m_activeScene; }
    const std::vector<std::unique_ptr<Scene>>& GetScenes() const { return m_scenes; }
    
    // Layer management
    void PushLayer(std::unique_ptr<Layer> layer);
    void PushOverlay(std::unique_ptr<Layer> overlay);
    void PopLayer();
    void PopOverlay();
    
    // Engine state
    EngineState GetState() const { return m_state; }
    const EngineConfig& GetConfig() const { return m_config; }
    
    // Timing
    float GetDeltaTime() const { return m_deltaTime; }
    float GetTime() const { return m_time; }
    uint64_t GetFrameCount() const { return m_frameCount; }
    float GetFPS() const { return m_fps; }
    
    // Events
    using EventCallback = std::function<void()>;
    void OnUpdate(const EventCallback& callback) { m_updateCallbacks.push_back(callback); }
    void OnRender(const EventCallback& callback) { m_renderCallbacks.push_back(callback); }
    void OnShutdown(const EventCallback& callback) { m_shutdownCallbacks.push_back(callback); }
    
private:
    bool InitializeSubsystems();
    void ShutdownSubsystems();
    void UpdateFPS();
    void ProcessEvents();
    
    static Engine* s_instance;
    
    EngineState m_state = EngineState::Uninitialized;
    EngineConfig m_config;
    bool m_shouldShutdown = false;
    
    // Core subsystems
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<PhysicsWorld> m_physics;
    std::unique_ptr<AudioEngine> m_audio;
    std::unique_ptr<AssetManager> m_assets;
    std::unique_ptr<ScriptEngine> m_scripting;
    
    // Application and scenes
    std::unique_ptr<Application> m_application;
    std::vector<std::unique_ptr<Scene>> m_scenes;
    Scene* m_activeScene = nullptr;
    
    // Layer stack
    std::vector<std::unique_ptr<Layer>> m_layers;
    std::vector<std::unique_ptr<Layer>> m_overlays;
    
    // Timing
    float m_deltaTime = 0.0f;
    float m_time = 0.0f;
    uint64_t m_frameCount = 0;
    float m_fps = 0.0f;
    float m_lastFrameTime = 0.0f;
    float m_fpsUpdateTimer = 0.0f;
    
    // Event callbacks
    std::vector<EventCallback> m_updateCallbacks;
    std::vector<EventCallback> m_renderCallbacks;
    std::vector<EventCallback> m_shutdownCallbacks;
};

} // namespace Demoph
