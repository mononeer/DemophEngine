
# Demoph Engine API Reference

This document provides comprehensive API documentation for Demoph Engine v1.0.

## Table of Contents

- [Core Classes](#core-classes)
- [Entity-Component-System](#entity-component-system)
- [Rendering](#rendering)
- [Physics](#physics)
- [Audio](#audio)
- [Input](#input)
- [Assets](#assets)
- [Scripting](#scripting)
- [Utilities](#utilities)

## Core Classes

### Engine

The main engine class that manages all subsystems.

```cpp
namespace Demoph {
    class Engine {
    public:
        // Factory method
        static std::unique_ptr<Engine> Create();
        static Engine* Get();
        
        // Lifecycle
        bool Initialize();
        bool Initialize(const EngineConfig& config);
        void Run();
        void Shutdown();
        
        // Frame updates
        void Update(float deltaTime);
        void Render();
        
        // Subsystem access
        Renderer* GetRenderer() const;
        PhysicsWorld* GetPhysics() const;
        AudioEngine* GetAudio() const;
        AssetManager* GetAssets() const;
        ScriptEngine* GetScripting() const;
        Window* GetWindow() const;
        
        // Scene management
        Scene* CreateScene(const std::string& name);
        void DestroyScene(Scene* scene);
        void SetActiveScene(Scene* scene);
        Scene* GetActiveScene() const;
        
        // Application
        void SetApplication(std::unique_ptr<Application> app);
        Application* GetApplication() const;
        
        // State
        EngineState GetState() const;
        float GetDeltaTime() const;
        float GetTime() const;
        uint64_t GetFrameCount() const;
        float GetFPS() const;
    };
}
```

#### EngineConfig

Configuration structure for engine initialization.

```cpp
struct EngineConfig {
    // Window settings
    std::string windowTitle = "Demoph Engine";
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    bool windowFullscreen = false;
    bool windowVSync = true;
    bool windowResizable = true;
    
    // Graphics settings
    std::string graphicsAPI = "auto"; // "vulkan", "opengl", "auto"
    uint32_t msaaSamples = 4;
    bool enableVSync = true;
    
    // Physics settings
    bool enablePhysics = true;
    float physicsTimeStep = 1.0f / 60.0f;
    
    // Audio settings
    bool enableAudio = true;
    float masterVolume = 1.0f;
    
    // Scripting settings
    bool enableScripting = true;
    std::string scriptingLanguage = "python";
    
    // Paths
    std::string assetsPath = "assets/";
    std::string shadersPath = "shaders/";
    std::string configPath = "config/";
    
    // Features
    bool enableImGui = true;
    bool enableProfiler = true;
    bool enableLogging = true;
};
```

### Application

Base class for game applications.

```cpp
class Application {
public:
    Application() = default;
    virtual ~Application() = default;
    
    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    virtual void Shutdown() {}
    
    Engine* GetEngine() const { return m_engine; }
    
private:
    friend class Engine;
    Engine* m_engine = nullptr;
};
```

### Scene

Container for entities and scene-specific logic.

```cpp
class Scene {
public:
    Scene(const std::string& name);
    ~Scene();
    
    // Entity management
    Entity* CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(Entity* entity);
    Entity* FindEntity(const std::string& name);
    const std::vector<std::unique_ptr<Entity>>& GetEntities() const;
    
    // Scene operations
    void Update(float deltaTime);
    void Render(Renderer* renderer);
    
    // Properties
    const std::string& GetName() const;
    void SetName(const std::string& name);
    bool IsActive() const;
    void SetActive(bool active);
    
    // Systems
    template<typename T, typename... Args>
    T* AddSystem(Args&&... args);
    
    template<typename T>
    T* GetSystem();
    
    template<typename T>
    void RemoveSystem();
};
```

## Entity-Component-System

### Entity

Represents a game object in the scene.

```cpp
class Entity {
public:
    Entity(Scene* scene, const std::string& name = "Entity");
    ~Entity();
    
    // Component management
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);
    
    template<typename T>
    T* GetComponent();
    
    template<typename T>
    bool HasComponent() const;
    
    template<typename T>
    void RemoveComponent();
    
    // Properties
    uint32_t GetID() const;
    const std::string& GetName() const;
    void SetName(const std::string& name);
    bool IsActive() const;
    void SetActive(bool active);
    Scene* GetScene() const;
    
    // Hierarchy
    void SetParent(Entity* parent);
    Entity* GetParent() const;
    void AddChild(Entity* child);
    void RemoveChild(Entity* child);
    const std::vector<Entity*>& GetChildren() const;
    
    // Transform shortcuts
    Transform* GetTransform() const;
};
```

### Component

Base class for all components.

```cpp
class Component {
public:
    Component() = default;
    virtual ~Component() = default;
    
    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    virtual void Shutdown() {}
    
    Entity* GetEntity() const { return m_entity; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
private:
    friend class Entity;
    Entity* m_entity = nullptr;
    bool m_enabled = true;
};
```

### System

Base class for systems that operate on components.

```cpp
class System {
public:
    System() = default;
    virtual ~System() = default;
    
    virtual void Initialize() {}
    virtual void Update(const std::vector<Entity*>& entities, float deltaTime) = 0;
    virtual void Shutdown() {}
    
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    int GetPriority() const { return m_priority; }
    void SetPriority(int priority) { m_priority = priority; }
    
private:
    bool m_enabled = true;
    int m_priority = 0;
};
```

## Rendering

### Renderer

Main rendering interface.

```cpp
class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool Initialize(Window* window, const std::string& api = "auto");
    void Shutdown();
    
    // Frame operations
    void BeginFrame();
    void EndFrame();
    void Clear(const glm::vec4& color = {0.1f, 0.1f, 0.1f, 1.0f});
    
    // Rendering
    void RenderEntity(Entity* entity);
    void RenderMesh(const Mesh& mesh, const Material& material, const glm::mat4& transform);
    void RenderSprite(const Texture& texture, const glm::vec2& position, const glm::vec2& size);
    
    // Camera
    void SetViewMatrix(const glm::mat4& view);
    void SetProjectionMatrix(const glm::mat4& projection);
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;
    
    // Lighting
    void SetAmbientLight(const glm::vec3& color);
    void AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
    void AddPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range);
    void AddSpotLight(const glm::vec3& position, const glm::vec3& direction, 
                     const glm::vec3& color, float intensity, float range, float angle);
    
    // Statistics
    uint32_t GetDrawCalls() const;
    uint32_t GetTriangles() const;
    void ResetStatistics();
};
```

### Camera

Camera component for rendering.

```cpp
class Camera : public Component {
public:
    enum class ProjectionType {
        Perspective,
        Orthographic
    };
    
    Camera();
    
    // Projection
    void SetPerspective(float fov, float aspect, float nearPlane, float farPlane);
    void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    
    // Properties
    ProjectionType GetProjectionType() const;
    float GetFieldOfView() const;
    float GetAspectRatio() const;
    float GetNearPlane() const;
    float GetFarPlane() const;
    
    // Matrices
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
    
    // Utility
    glm::vec3 ScreenToWorldPoint(const glm::vec2& screenPoint, float depth = 1.0f) const;
    glm::vec2 WorldToScreenPoint(const glm::vec3& worldPoint) const;
};
```

### Material

Material properties for rendering.

```cpp
class Material : public Component {
public:
    Material();
    
    // Shader
    void SetShader(const std::string& shaderPath);
    void SetShader(Shader* shader);
    Shader* GetShader() const;
    
    // Textures
    void SetTexture(const std::string& name, Texture* texture);
    void SetAlbedoTexture(Texture* texture);
    void SetNormalTexture(Texture* texture);
    void SetMetallicTexture(Texture* texture);
    void SetRoughnessTexture(Texture* texture);
    
    // Properties
    void SetAlbedo(const glm::vec4& color);
    void SetMetallic(float metallic);
    void SetRoughness(float roughness);
    void SetEmission(const glm::vec3& emission);
    
    // Parameters
    template<typename T>
    void SetParameter(const std::string& name, const T& value);
    
    template<typename T>
    T GetParameter(const std::string& name) const;
};
```

### MeshRenderer

Component for rendering 3D meshes.

```cpp
class MeshRenderer : public Component {
public:
    MeshRenderer();
    
    void SetMesh(const std::string& meshPath);
    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;
    
    void SetMaterial(Material* material);
    Material* GetMaterial() const;
    
    void SetCastShadows(bool cast);
    bool GetCastShadows() const;
    
    void SetReceiveShadows(bool receive);
    bool GetReceiveShadows() const;
    
    void Render() override;
};
```

## Physics

### PhysicsWorld

Main physics simulation world.

```cpp
class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();
    
    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    
    // World properties
    void SetGravity(const glm::vec3& gravity);
    glm::vec3 GetGravity() const;
    
    // Raycasting
    struct RaycastHit {
        Entity* entity = nullptr;
        glm::vec3 point;
        glm::vec3 normal;
        float distance = 0.0f;
    };
    
    bool Raycast(const glm::vec3& origin, const glm::vec3& direction, 
                float maxDistance, RaycastHit& hit);
    std::vector<RaycastHit> RaycastAll(const glm::vec3& origin, const glm::vec3& direction, 
                                      float maxDistance);
    
    // Collision queries
    std::vector<Entity*> OverlapSphere(const glm::vec3& center, float radius);
    std::vector<Entity*> OverlapBox(const glm::vec3& center, const glm::vec3& halfExtents);
};
```

### RigidBody

Physics rigid body component.

```cpp
class RigidBody : public Component {
public:
    enum class Type {
        Static,
        Kinematic,
        Dynamic
    };
    
    RigidBody();
    
    // Type
    void SetType(Type type);
    Type GetType() const;
    
    // Mass
    void SetMass(float mass);
    float GetMass() const;
    
    // Damping
    void SetLinearDamping(float damping);
    float GetLinearDamping() const;
    void SetAngularDamping(float damping);
    float GetAngularDamping() const;
    
    // Forces
    void AddForce(const glm::vec3& force);
    void AddForceAtPosition(const glm::vec3& force, const glm::vec3& position);
    void AddTorque(const glm::vec3& torque);
    void AddImpulse(const glm::vec3& impulse);
    void AddImpulseAtPosition(const glm::vec3& impulse, const glm::vec3& position);
    
    // Velocity
    void SetLinearVelocity(const glm::vec3& velocity);
    glm::vec3 GetLinearVelocity() const;
    void SetAngularVelocity(const glm::vec3& velocity);
    glm::vec3 GetAngularVelocity() const;
    
    // Constraints
    void SetFreezePosition(bool x, bool y, bool z);
    void SetFreezeRotation(bool x, bool y, bool z);
};
```

### Collider

Base class for collision shapes.

```cpp
class Collider : public Component {
public:
    Collider();
    
    // Material
    void SetPhysicsMaterial(PhysicsMaterial* material);
    PhysicsMaterial* GetPhysicsMaterial() const;
    
    // Trigger
    void SetIsTrigger(bool trigger);
    bool IsTrigger() const;
    
    // Events
    std::function<void(Entity*)> OnCollisionEnter;
    std::function<void(Entity*)> OnCollisionStay;
    std::function<void(Entity*)> OnCollisionExit;
    std::function<void(Entity*)> OnTriggerEnter;
    std::function<void(Entity*)> OnTriggerStay;
    std::function<void(Entity*)> OnTriggerExit;
};

class BoxCollider : public Collider {
public:
    void SetSize(const glm::vec3& size);
    glm::vec3 GetSize() const;
};

class SphereCollider : public Collider {
public:
    void SetRadius(float radius);
    float GetRadius() const;
};

class CapsuleCollider : public Collider {
public:
    void SetRadius(float radius);
    void SetHeight(float height);
    float GetRadius() const;
    float GetHeight() const;
};

class MeshCollider : public Collider {
public:
    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;
    void SetConvex(bool convex);
    bool IsConvex() const;
};
```

## Audio

### AudioEngine

Main audio system.

```cpp
class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    
    // Master controls
    void SetMasterVolume(float volume);
    float GetMasterVolume() const;
    
    // Listener (camera)
    void SetListenerPosition(const glm::vec3& position);
    void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up);
    void SetListenerVelocity(const glm::vec3& velocity);
    
    // Global audio settings
    void SetDopplerFactor(float factor);
    void SetSpeedOfSound(float speed);
};
```

### AudioSource

3D audio source component.

```cpp
class AudioSource : public Component {
public:
    AudioSource();
    
    // Clip
    void SetClip(const std::string& clipPath);
    void SetClip(AudioClip* clip);
    AudioClip* GetClip() const;
    
    // Playback
    void Play();
    void Pause();
    void Stop();
    bool IsPlaying() const;
    bool IsPaused() const;
    
    // Properties
    void SetVolume(float volume);
    float GetVolume() const;
    void SetPitch(float pitch);
    float GetPitch() const;
    void SetLooping(bool loop);
    bool IsLooping() const;
    
    // 3D audio
    void SetMinDistance(float distance);
    float GetMinDistance() const;
    void SetMaxDistance(float distance);
    float GetMaxDistance() const;
    void SetRolloffFactor(float factor);
    float GetRolloffFactor() const;
    
    // Events
    std::function<void()> OnPlaybackComplete;
};
```

## Input

### Input

Static input system for querying input state.

```cpp
class Input {
public:
    // Keyboard
    static bool IsKeyDown(Key key);
    static bool IsKeyPressed(Key key);
    static bool IsKeyReleased(Key key);
    
    // Mouse
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonPressed(MouseButton button);
    static bool IsMouseButtonReleased(MouseButton button);
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();
    static float GetMouseScrollDelta();
    
    // Gamepad
    static bool IsGamepadConnected(int gamepadId);
    static bool IsGamepadButtonDown(int gamepadId, GamepadButton button);
    static bool IsGamepadButtonPressed(int gamepadId, GamepadButton button);
    static bool IsGamepadButtonReleased(int gamepadId, GamepadButton button);
    static float GetGamepadAxis(int gamepadId, GamepadAxis axis);
    static glm::vec2 GetGamepadStick(int gamepadId, GamepadStick stick);
    
    // System
    static void Initialize(Window* window);
    static void Update();
    static void Shutdown();
};

enum class Key {
    Unknown = -1,
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Key0 = 48, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
    Semicolon = 59,
    Equal = 61,
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    Escape = 256,
    Enter, Tab, Backspace, Insert, Delete, Right, Left, Down, Up,
    PageUp, PageDown, Home, End,
    CapsLock = 280,
    ScrollLock, NumLock, PrintScreen, Pause,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    LeftShift = 340,
    LeftControl, LeftAlt, LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper
};

enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};
```

## Assets

### AssetManager

Centralized asset loading and management.

```cpp
class AssetManager {
public:
    AssetManager();
    ~AssetManager();
    
    bool Initialize(const std::string& assetsPath);
    void Shutdown();
    
    // Loading
    template<typename T>
    std::shared_ptr<T> Load(const std::string& path);
    
    template<typename T>
    std::shared_ptr<T> LoadAsync(const std::string& path);
    
    // Management
    template<typename T>
    void Unload(const std::string& path);
    
    void UnloadAll();
    void UnloadUnused();
    
    // Queries
    template<typename T>
    bool IsLoaded(const std::string& path) const;
    
    template<typename T>
    std::shared_ptr<T> Get(const std::string& path) const;
    
    // Hot reload
    void EnableHotReload(bool enable);
    bool IsHotReloadEnabled() const;
    void CheckForChanges();
    
    // Statistics
    size_t GetLoadedAssetsCount() const;
    size_t GetMemoryUsage() const;
};
```

### Asset Types

```cpp
// Base asset class
class Asset {
public:
    Asset(const std::string& path);
    virtual ~Asset() = default;
    
    const std::string& GetPath() const;
    bool IsLoaded() const;
    virtual bool Load() = 0;
    virtual void Unload() = 0;
};

// Texture asset
class Texture : public Asset {
public:
    enum class Format {
        RGB, RGBA, R, RG,
        RGB16F, RGBA16F,
        RGB32F, RGBA32F,
        Depth24, Depth32F
    };
    
    enum class Filter {
        Nearest, Linear,
        NearestMipmapNearest, LinearMipmapNearest,
        NearestMipmapLinear, LinearMipmapLinear
    };
    
    enum class Wrap {
        Repeat, MirroredRepeat,
        ClampToEdge, ClampToBorder
    };
    
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    Format GetFormat() const;
    
    void SetFilter(Filter minFilter, Filter magFilter);
    void SetWrap(Wrap wrapS, Wrap wrapT);
    void GenerateMipmaps();
};

// Mesh asset
class Mesh : public Asset {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoords;
    };
    
    const std::vector<Vertex>& GetVertices() const;
    const std::vector<uint32_t>& GetIndices() const;
    
    glm::vec3 GetMinBounds() const;
    glm::vec3 GetMaxBounds() const;
    glm::vec3 GetCenter() const;
    float GetRadius() const;
};

// Audio clip asset
class AudioClip : public Asset {
public:
    float GetDuration() const;
    uint32_t GetChannels() const;
    uint32_t GetSampleRate() const;
    uint32_t GetBitsPerSample() const;
};
```

## Scripting

### ScriptEngine

Python/Lua scripting integration.

```cpp
class ScriptEngine {
public:
    ScriptEngine();
    ~ScriptEngine();
    
    bool Initialize(const std::string& language = "python");
    void Shutdown();
    void Update(float deltaTime);
    
    // Script execution
    bool ExecuteFile(const std::string& path);
    bool ExecuteString(const std::string& code);
    
    // Module management
    bool LoadModule(const std::string& name, const std::string& path);
    void UnloadModule(const std::string& name);
    bool ReloadModule(const std::string& name);
    
    // Variable access
    template<typename T>
    void SetGlobalVariable(const std::string& name, const T& value);
    
    template<typename T>
    T GetGlobalVariable(const std::string& name) const;
    
    // Function calls
    template<typename ReturnType, typename... Args>
    ReturnType CallFunction(const std::string& name, Args&&... args);
    
    // Hot reload
    void EnableHotReload(bool enable);
    void CheckForChanges();
};
```

### ScriptComponent

Component for attaching scripts to entities.

```cpp
class ScriptComponent : public Component {
public:
    ScriptComponent();
    
    void SetScript(const std::string& scriptPath);
    const std::string& GetScriptPath() const;
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    // Script interface
    template<typename T>
    void SetVariable(const std::string& name, const T& value);
    
    template<typename T>
    T GetVariable(const std::string& name) const;
    
    template<typename ReturnType, typename... Args>
    ReturnType CallMethod(const std::string& name, Args&&... args);
    
    // Events
    void OnCollisionEnter(Entity* other);
    void OnCollisionExit(Entity* other);
    void OnTriggerEnter(Entity* other);
    void OnTriggerExit(Entity* other);
};
```

## Utilities

### Math

Extended math utilities.

```cpp
namespace Math {
    // Constants
    constexpr float PI = 3.14159265359f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = PI * 0.5f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
    
    // Conversion
    float ToRadians(float degrees);
    float ToDegrees(float radians);
    
    // Interpolation
    float Lerp(float a, float b, float t);
    glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t);
    glm::quat Slerp(const glm::quat& a, const glm::quat& b, float t);
    
    // Clamping
    float Clamp(float value, float min, float max);
    glm::vec3 Clamp(const glm::vec3& value, const glm::vec3& min, const glm::vec3& max);
    
    // Easing functions
    float EaseInSine(float t);
    float EaseOutSine(float t);
    float EaseInOutSine(float t);
    // ... more easing functions
    
    // Random
    void SetRandomSeed(uint32_t seed);
    float Random(); // 0.0 to 1.0
    float Random(float min, float max);
    int RandomInt(int min, int max);
    glm::vec3 RandomDirection();
    glm::vec3 RandomInSphere();
    
    // Geometry
    bool IntersectRayPlane(const glm::vec3& origin, const glm::vec3& direction,
                          const glm::vec3& planePoint, const glm::vec3& planeNormal,
                          float& distance);
    
    bool IntersectRaySphere(const glm::vec3& origin, const glm::vec3& direction,
                           const glm::vec3& sphereCenter, float sphereRadius,
                           float& distance);
}
```

### Timer

High-precision timing utilities.

```cpp
class Timer {
public:
    Timer();
    
    void Start();
    void Stop();
    void Reset();
    void Restart();
    
    bool IsRunning() const;
    float GetElapsedTime() const; // in seconds
    uint64_t GetElapsedTimeMs() const; // in milliseconds
    uint64_t GetElapsedTimeUs() const; // in microseconds
    
    // Static utilities
    static float GetTime(); // Time since engine start
    static uint64_t GetTimeMs();
    static uint64_t GetTimeUs();
    static void Sleep(uint32_t milliseconds);
};
```

### FileSystem

Cross-platform file system utilities.

```cpp
namespace FileSystem {
    // Path operations
    std::string GetWorkingDirectory();
    std::string GetExecutableDirectory();
    std::string JoinPath(const std::string& path1, const std::string& path2);
    std::string GetDirectory(const std::string& path);
    std::string GetFilename(const std::string& path);
    std::string GetExtension(const std::string& path);
    std::string RemoveExtension(const std::string& path);
    
    // File operations
    bool Exists(const std::string& path);
    bool IsFile(const std::string& path);
    bool IsDirectory(const std::string& path);
    uint64_t GetFileSize(const std::string& path);
    uint64_t GetLastWriteTime(const std::string& path);
    
    bool CreateDirectory(const std::string& path);
    bool DeleteFile(const std::string& path);
    bool DeleteDirectory(const std::string& path);
    bool CopyFile(const std::string& source, const std::string& destination);
    bool MoveFile(const std::string& source, const std::string& destination);
    
    // Directory listing
    std::vector<std::string> ListDirectory(const std::string& path);
    std::vector<std::string> ListFiles(const std::string& path, const std::string& extension = "");
    std::vector<std::string> ListDirectories(const std::string& path);
    
    // File content
    std::string ReadTextFile(const std::string& path);
    std::vector<uint8_t> ReadBinaryFile(const std::string& path);
    bool WriteTextFile(const std::string& path, const std::string& content);
    bool WriteBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
    
    // Watching
    class FileWatcher {
    public:
        FileWatcher(const std::string& path);
        ~FileWatcher();
        
        bool HasChanged();
        std::vector<std::string> GetChangedFiles();
        
        using ChangeCallback = std::function<void(const std::string&)>;
        void SetCallback(const ChangeCallback& callback);
    };
}
```

This API reference covers the core functionality of Demoph Engine v1.0. For more detailed information and examples, refer to the individual class documentation and tutorials.
