
# Getting Started with Demoph Engine

Welcome to Demoph Engine - Where Precision Meets Possibility! This guide will help you get up and running with the engine in no time.

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Installation](#installation)
3. [Building from Source](#building-from-source)
4. [Your First Project](#your-first-project)
5. [Project Structure](#project-structure)
6. [Basic Concepts](#basic-concepts)
7. [Next Steps](#next-steps)

## System Requirements

### Minimum Requirements
- **OS**: Windows 10, macOS 10.15, or Linux (Ubuntu 20.04+)
- **CPU**: 64-bit processor with SSE4.1 support
- **RAM**: 4 GB RAM
- **GPU**: DirectX 11 / OpenGL 4.3 compatible graphics card
- **Storage**: 2 GB available space

### Recommended Requirements
- **OS**: Windows 11, macOS 12+, or Linux (Ubuntu 22.04+)
- **CPU**: Multi-core 64-bit processor (Intel i5/AMD Ryzen 5 or better)
- **RAM**: 8 GB RAM or more
- **GPU**: Vulkan-compatible graphics card with 2 GB VRAM
- **Storage**: 5 GB available space (SSD recommended)

### Development Requirements
- **CMake**: 3.20 or higher
- **Compiler**: 
  - Windows: Visual Studio 2022 or MinGW-w64
  - macOS: Xcode 14+ or Clang 13+
  - Linux: GCC 11+ or Clang 13+
- **Python**: 3.11+ (for scripting support)
- **Git**: For version control and dependency management

## Installation

### Option 1: Pre-built Binaries (Recommended)

1. Download the latest release from the [Releases page](https://github.com/demoph/engine/releases)
2. Extract the archive to your desired location
3. Add the `bin` directory to your system PATH
4. Verify installation by running:
   ```bash
   demoph --version
   ```

### Option 2: Package Managers

#### Windows (vcpkg)
```bash
vcpkg install demoph-engine
```

#### macOS (Homebrew)
```bash
brew install demoph-engine
```

#### Linux (APT)
```bash
sudo apt-get install demoph-engine-dev
```

## Building from Source

### 1. Clone the Repository
```bash
git clone https://github.com/demoph/engine.git
cd engine
git submodule update --init --recursive
```

### 2. Install Dependencies

#### Windows
```bash
# Using vcpkg (recommended)
vcpkg install vulkan-headers vulkan-loader glfw3 glm bullet3 openal-soft assimp
```

#### macOS
```bash
# Using Homebrew
brew install cmake vulkan-headers vulkan-loader glfw glm bullet openal-soft assimp
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libvulkan-dev libglfw3-dev libglm-dev libbullet-dev libopenal-dev libassimp-dev
```

### 3. Configure and Build
```bash
mkdir build && cd build

# Configure (Release build)
cmake .. -DCMAKE_BUILD_TYPE=Release -DDEMOPH_BUILD_EXAMPLES=ON

# Build
cmake --build . --parallel

# Install (optional)
sudo cmake --install .
```

### 4. Build Options

You can customize the build with these CMake options:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DDEMOPH_BUILD_EXAMPLES=ON \
  -DDEMOPH_BUILD_TESTS=ON \
  -DDEMOPH_BUILD_TOOLS=ON \
  -DDEMOPH_ENABLE_VULKAN=ON \
  -DDEMOPH_ENABLE_OPENGL=ON \
  -DDEMOPH_ENABLE_PYTHON=ON \
  -DDEMOPH_BUILD_SHARED=ON
```

## Your First Project

### 1. Create a New Project

```bash
mkdir my-game && cd my-game
demoph create-project MyGame
```

This creates a basic project structure:

```
my-game/
├── CMakeLists.txt
├── main.cpp
├── assets/
│   ├── models/
│   ├── textures/
│   ├── sounds/
│   └── shaders/
├── scripts/
└── config/
    └── engine.json
```

### 2. Basic Application (main.cpp)

```cpp
#include <demoph/demoph.h>

class MyGameApp : public Demoph::Application {
public:
    MyGameApp() = default;
    
    void Initialize() override {
        // Create a scene
        auto scene = GetEngine()->CreateScene("MainScene");
        GetEngine()->SetActiveScene(scene);
        
        // Create a camera
        auto cameraEntity = scene->CreateEntity("Camera");
        auto transform = cameraEntity->AddComponent<Demoph::Transform>();
        transform->position = {0, 0, 5};
        
        auto camera = cameraEntity->AddComponent<Demoph::Camera>();
        camera->SetPerspective(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        
        // Create a cube
        auto cubeEntity = scene->CreateEntity("Cube");
        cubeEntity->AddComponent<Demoph::Transform>();
        
        auto meshRenderer = cubeEntity->AddComponent<Demoph::MeshRenderer>();
        meshRenderer->SetMesh("primitives/cube");
        
        auto material = cubeEntity->AddComponent<Demoph::Material>();
        material->SetShader("pbr");
        material->SetAlbedo({1.0f, 0.5f, 0.2f, 1.0f});
        
        // Add a light
        auto lightEntity = scene->CreateEntity("Light");
        auto lightTransform = lightEntity->AddComponent<Demoph::Transform>();
        lightTransform->position = {2, 2, 2};
        
        auto light = lightEntity->AddComponent<Demoph::Light>();
        light->type = Demoph::LightType::Directional;
        light->color = {1, 1, 1};
        light->intensity = 1.0f;
    }
    
    void Update(float deltaTime) override {
        // Game logic here
        if (Demoph::Input::IsKeyPressed(Demoph::Key::Escape)) {
            GetEngine()->RequestShutdown();
        }
    }
    
    void Render() override {
        // Custom rendering here (optional)
    }
};

int main() {
    // Configure engine
    Demoph::EngineConfig config;
    config.windowTitle = "My First Game";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    
    // Create and initialize engine
    auto engine = Demoph::Engine::Create();
    if (!engine->Initialize(config)) {
        return -1;
    }
    
    // Set application
    engine->SetApplication(std::make_unique<MyGameApp>());
    
    // Run
    engine->Run();
    
    return 0;
}
```

### 3. Build and Run

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
./MyGame
```

## Project Structure

### Recommended Directory Layout

```
my-project/
├── CMakeLists.txt          # Build configuration
├── main.cpp               # Application entry point
├── src/                   # Source code
│   ├── game/              # Game-specific code
│   ├── components/        # Custom components
│   └── systems/           # Custom systems
├── include/               # Header files
├── assets/                # Game assets
│   ├── models/            # 3D models (.fbx, .obj, .gltf)
│   ├── textures/          # Textures (.png, .jpg, .hdr)
│   ├── sounds/            # Audio files (.wav, .ogg, .mp3)
│   ├── music/             # Music files
│   ├── fonts/             # Font files (.ttf, .otf)
│   └── data/              # Game data (.json, .xml)
├── shaders/               # Custom shaders
│   ├── vertex/            # Vertex shaders (.vert)
│   ├── fragment/          # Fragment shaders (.frag)
│   └── compute/           # Compute shaders (.comp)
├── scripts/               # Python/Lua scripts
├── config/                # Configuration files
│   ├── engine.json        # Engine settings
│   ├── graphics.json      # Graphics settings
│   └── input.json         # Input mappings
├── docs/                  # Documentation
└── external/              # Third-party dependencies
```

## Basic Concepts

### Entity-Component-System (ECS)

Demoph Engine uses an Entity-Component-System architecture:

```cpp
// Create an entity
auto entity = scene->CreateEntity("Player");

// Add components
auto transform = entity->AddComponent<Transform>();
auto meshRenderer = entity->AddComponent<MeshRenderer>();
auto rigidBody = entity->AddComponent<RigidBody>();

// Configure components
transform->position = {0, 0, 0};
meshRenderer->SetMesh("models/player.fbx");
rigidBody->SetMass(1.0f);
```

### Scenes

Scenes organize your game objects:

```cpp
// Create a scene
auto mainScene = engine->CreateScene("MainScene");
auto menuScene = engine->CreateScene("MenuScene");

// Switch scenes
engine->SetActiveScene(mainScene);
```

### Assets

The engine provides automatic asset management:

```cpp
// Load assets (automatically cached)
auto texture = assets->Load<Texture>("textures/player.png");
auto model = assets->Load<Model>("models/character.fbx");
auto sound = assets->Load<AudioClip>("sounds/jump.wav");

// Use with components
meshRenderer->SetTexture(texture);
audioSource->SetClip(sound);
```

### Scripting

Add Python scripts to entities:

```cpp
// C++ side
auto scriptComponent = entity->AddComponent<ScriptComponent>();
scriptComponent->SetScript("scripts/player_controller.py");
```

```python
# scripts/player_controller.py
import demoph

class PlayerController(demoph.ScriptComponent):
    def __init__(self):
        self.speed = 5.0
    
    def update(self, dt):
        if demoph.input.is_key_down(demoph.Key.W):
            self.transform.position.z -= self.speed * dt
        if demoph.input.is_key_down(demoph.Key.S):
            self.transform.position.z += self.speed * dt
```

### Rendering

The engine supports both 2D and 3D rendering:

```cpp
// 3D rendering
auto meshRenderer = entity->AddComponent<MeshRenderer>();
meshRenderer->SetMesh("models/building.fbx");

auto material = entity->AddComponent<Material>();
material->SetShader("pbr");
material->SetAlbedo({0.8f, 0.8f, 0.8f, 1.0f});
material->SetMetallic(0.0f);
material->SetRoughness(0.8f);

// 2D rendering
auto spriteRenderer = entity->AddComponent<SpriteRenderer>();
spriteRenderer->SetTexture("textures/character.png");
spriteRenderer->SetSize({64, 64});
```

### Physics

Bullet Physics integration for realistic simulation:

```cpp
auto rigidBody = entity->AddComponent<RigidBody>();
rigidBody->SetMass(1.0f);
rigidBody->SetLinearDamping(0.1f);

auto collider = entity->AddComponent<BoxCollider>();
collider->SetSize({1, 1, 1});

// Apply forces
rigidBody->AddForce({0, 100, 0}); // Jump
rigidBody->AddTorque({0, 50, 0}); // Spin
```

### Audio

3D spatial audio with environmental effects:

```cpp
auto audioSource = entity->AddComponent<AudioSource>();
audioSource->SetClip("sounds/engine.wav");
audioSource->SetLooping(true);
audioSource->SetVolume(0.8f);
audioSource->SetPitch(1.2f);
audioSource->Play();

// 3D audio properties
audioSource->SetMinDistance(1.0f);
audioSource->SetMaxDistance(100.0f);
audioSource->SetRolloffFactor(1.0f);
```

## Next Steps

Congratulations! You now have a basic understanding of Demoph Engine. Here's what to explore next:

### Tutorials
- [Creating a 3D Scene](tutorials/3d-scene.md)
- [Physics Simulation](tutorials/physics.md)
- [Audio Integration](tutorials/audio.md)
- [Python Scripting](tutorials/scripting.md)
- [Custom Shaders](tutorials/shaders.md)

### Examples
- [Platformer Game](examples/platformer/)
- [Racing Game](examples/racing/)
- [RPG Demo](examples/rpg/)
- [Physics Playground](examples/physics/)

### Advanced Topics
- [Engine Architecture](architecture.md)
- [Performance Optimization](performance.md)
- [Custom Components](custom-components.md)
- [Editor Tools](editor-tools.md)

### Community
- [Discord Server](https://discord.gg/demoph)
- [GitHub Discussions](https://github.com/demoph/engine/discussions)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/demoph-engine)

Happy game development! 🎮
