
#pragma once

/**
 * @file demoph.h
 * @brief Main Demoph Engine Header - Where Precision Meets Possibility
 * @version 1.0.0
 * @author Demoph Studios
 * 
 * This is the main header file for the Demoph Engine, providing access to all
 * engine subsystems and components. Include this file to use the engine.
 */

// Platform detection and configuration
#ifdef _WIN32
    #define DEMOPH_PLATFORM_WINDOWS
    #ifdef DEMOPH_BUILD_DLL
        #define DEMOPH_API __declspec(dllexport)
    #else
        #define DEMOPH_API __declspec(dllimport)
    #endif
#elif defined(__linux__)
    #define DEMOPH_PLATFORM_LINUX
    #define DEMOPH_API __attribute__((visibility("default")))
#elif defined(__APPLE__)
    #define DEMOPH_PLATFORM_MACOS
    #define DEMOPH_API __attribute__((visibility("default")))
#else
    #error "Unsupported platform"
#endif

// Compiler detection
#if defined(__clang__)
    #define DEMOPH_COMPILER_CLANG
#elif defined(__GNUC__)
    #define DEMOPH_COMPILER_GCC
#elif defined(_MSC_VER)
    #define DEMOPH_COMPILER_MSVC
#endif

// C++ standard library includes
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <queue>
#include <stack>
#include <array>
#include <span>
#include <optional>
#include <variant>
#include <format>

// Math library
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

// Core engine headers
#include "demoph/core/engine.h"
#include "demoph/core/application.h"
#include "demoph/core/scene.h"
#include "demoph/core/entity.h"
#include "demoph/core/component.h"
#include "demoph/core/system.h"
#include "demoph/core/layer.h"
#include "demoph/core/timestep.h"
#include "demoph/core/log.h"
#include "demoph/core/assert.h"
#include "demoph/core/event.h"
#include "demoph/core/input.h"
#include "demoph/core/window.h"

// ECS (Entity Component System)
#include "demoph/ecs/world.h"
#include "demoph/ecs/entity_manager.h"
#include "demoph/ecs/component_manager.h"
#include "demoph/ecs/system_manager.h"
#include "demoph/ecs/archetype.h"

// Renderer
#include "demoph/renderer/renderer.h"
#include "demoph/renderer/renderer2d.h"
#include "demoph/renderer/renderer3d.h"
#include "demoph/renderer/shader.h"
#include "demoph/renderer/texture.h"
#include "demoph/renderer/material.h"
#include "demoph/renderer/mesh.h"
#include "demoph/renderer/model.h"
#include "demoph/renderer/camera.h"
#include "demoph/renderer/framebuffer.h"
#include "demoph/renderer/vertex_array.h"
#include "demoph/renderer/vertex_buffer.h"
#include "demoph/renderer/index_buffer.h"
#include "demoph/renderer/uniform_buffer.h"
#include "demoph/renderer/storage_buffer.h"
#include "demoph/renderer/render_pass.h"
#include "demoph/renderer/pipeline.h"

// Graphics API abstraction
#include "demoph/renderer/graphics_context.h"
#include "demoph/renderer/command_buffer.h"
#include "demoph/renderer/descriptor_set.h"

// Physics
#include "demoph/physics/physics_world.h"
#include "demoph/physics/rigid_body.h"
#include "demoph/physics/collider.h"
#include "demoph/physics/physics_material.h"
#include "demoph/physics/constraint.h"
#include "demoph/physics/raycast.h"

// Audio
#include "demoph/audio/audio_engine.h"
#include "demoph/audio/audio_source.h"
#include "demoph/audio/audio_listener.h"
#include "demoph/audio/audio_buffer.h"
#include "demoph/audio/audio_effect.h"

// Asset Management
#include "demoph/assets/asset_manager.h"
#include "demoph/assets/asset_loader.h"
#include "demoph/assets/mesh_loader.h"
#include "demoph/assets/texture_loader.h"
#include "demoph/assets/audio_loader.h"
#include "demoph/assets/shader_loader.h"
#include "demoph/assets/scene_loader.h"

// Scripting
#include "demoph/scripting/script_engine.h"
#include "demoph/scripting/script_component.h"

// Animation
#include "demoph/animation/animator.h"
#include "demoph/animation/animation_clip.h"
#include "demoph/animation/bone.h"
#include "demoph/animation/skeleton.h"

// Particle System
#include "demoph/particles/particle_system.h"
#include "demoph/particles/particle_emitter.h"
#include "demoph/particles/particle.h"

// UI System
#include "demoph/ui/ui_manager.h"
#include "demoph/ui/ui_element.h"
#include "demoph/ui/button.h"
#include "demoph/ui/text.h"
#include "demoph/ui/image.h"
#include "demoph/ui/panel.h"

// Networking
#include "demoph/network/network_manager.h"
#include "demoph/network/client.h"
#include "demoph/network/server.h"
#include "demoph/network/packet.h"

// Utilities
#include "demoph/utils/file_system.h"
#include "demoph/utils/timer.h"
#include "demoph/utils/math.h"
#include "demoph/utils/string_utils.h"
#include "demoph/utils/platform.h"
#include "demoph/utils/memory.h"
#include "demoph/utils/thread_pool.h"
#include "demoph/utils/profiler.h"

// Math types
#include "demoph/math/vec2.h"
#include "demoph/math/vec3.h"
#include "demoph/math/vec4.h"
#include "demoph/math/mat3.h"
#include "demoph/math/mat4.h"
#include "demoph/math/quaternion.h"
#include "demoph/math/transform.h"
#include "demoph/math/aabb.h"
#include "demoph/math/obb.h"
#include "demoph/math/plane.h"
#include "demoph/math/frustum.h"

// Utility macros
#define DEMOPH_NO_COPY(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define DEMOPH_NO_MOVE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define DEMOPH_NO_COPY_NO_MOVE(ClassName) \
    DEMOPH_NO_COPY(ClassName) \
    DEMOPH_NO_MOVE(ClassName)

#define DEMOPH_BIND_EVENT_FN(fn) \
    [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Version information
namespace Demoph {
    namespace Version {
        constexpr int Major = 1;
        constexpr int Minor = 0;
        constexpr int Patch = 0;
        constexpr const char* String = "1.0.0";
        constexpr const char* Tagline = "Where Precision Meets Possibility";
    }

    namespace BuildInfo {
        constexpr const char* Date = __DATE__;
        constexpr const char* Time = __TIME__;
        
        #ifdef DEMOPH_PLATFORM_WINDOWS
            constexpr const char* Platform = "Windows";
        #elif defined(DEMOPH_PLATFORM_LINUX)
            constexpr const char* Platform = "Linux";
        #elif defined(DEMOPH_PLATFORM_MACOS)
            constexpr const char* Platform = "macOS";
        #endif
        
        #ifdef NDEBUG
            constexpr const char* Configuration = "Release";
        #else
            constexpr const char* Configuration = "Debug";
        #endif
        
        #ifdef DEMOPH_COMPILER_CLANG
            constexpr const char* Compiler = "Clang";
        #elif defined(DEMOPH_COMPILER_GCC)
            constexpr const char* Compiler = "GCC";
        #elif defined(DEMOPH_COMPILER_MSVC)
            constexpr const char* Compiler = "MSVC";
        #endif
    }
}

// Forward declarations for commonly used types
namespace Demoph {
    class Engine;
    class Application;
    class Scene;
    class Entity;
    class Component;
    class System;
    class Layer;
    
    class Renderer;
    class Shader;
    class Texture;
    class Material;
    class Mesh;
    class Model;
    class Camera;
    
    class PhysicsWorld;
    class RigidBody;
    class Collider;
    
    class AudioEngine;
    class AudioSource;
    class AudioListener;
    
    class AssetManager;
    class ScriptEngine;
    
    // Math types
    class Vec2;
    class Vec3;
    class Vec4;
    class Mat3;
    class Mat4;
    class Quaternion;
    class Transform;
    
    // Smart pointer aliases
    template<typename T>
    using Ref = std::shared_ptr<T>;
    
    template<typename T>
    using Scope = std::unique_ptr<T>;
    
    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
    template<typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
}

// Engine initialization helper
#define DEMOPH_MAIN(AppClass) \
int main(int argc, char** argv) { \
    auto engine = Demoph::Engine::Create(); \
    if (!engine) { \
        return -1; \
    } \
    \
    Demoph::EngineConfig config; \
    if (!engine->Initialize(config)) { \
        return -1; \
    } \
    \
    engine->SetApplication(std::make_unique<AppClass>()); \
    engine->Run(); \
    \
    return 0; \
}
