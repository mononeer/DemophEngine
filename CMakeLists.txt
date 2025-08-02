
cmake_minimum_required(VERSION 3.20)

# Project definition
project(DemophEngine 
    VERSION 1.0.0 
    DESCRIPTION "Demoph Engine - Where Precision Meets Possibility"
    LANGUAGES CXX C
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable interprocedural optimization in Release
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Build configuration
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build options
option(DEMOPH_BUILD_EXAMPLES "Build example projects" ON)
option(DEMOPH_BUILD_TESTS "Build unit tests" ON)
option(DEMOPH_BUILD_TOOLS "Build editor tools" ON)
option(DEMOPH_BUILD_BENCHMARKS "Build performance benchmarks" ON)
option(DEMOPH_ENABLE_VULKAN "Enable Vulkan renderer" ON)
option(DEMOPH_ENABLE_OPENGL "Enable OpenGL renderer" ON)
option(DEMOPH_ENABLE_DIRECTX12 "Enable DirectX 12 renderer" OFF)
option(DEMOPH_ENABLE_PYTHON "Enable Python scripting" ON)
option(DEMOPH_ENABLE_LUA "Enable Lua scripting" OFF)
option(DEMOPH_ENABLE_CSHARP "Enable C# scripting" OFF)
option(DEMOPH_ENABLE_NETWORKING "Enable networking support" ON)
option(DEMOPH_ENABLE_VR "Enable VR support" OFF)
option(DEMOPH_ENABLE_PROFILING "Enable built-in profiler" ON)
option(DEMOPH_ENABLE_HOT_RELOAD "Enable hot reload" ON)
option(DEMOPH_BUILD_SHARED "Build as shared library" ON)
option(DEMOPH_ENABLE_TRACY "Enable Tracy profiler integration" OFF)
option(DEMOPH_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(DEMOPH_ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(DEMOPH_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# Platform detection
if(WIN32)
    set(DEMOPH_PLATFORM "Windows")
    add_compile_definitions(DEMOPH_PLATFORM_WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(APPLE)
    set(DEMOPH_PLATFORM "macOS")
    add_compile_definitions(DEMOPH_PLATFORM_MACOS)
elseif(UNIX)
    set(DEMOPH_PLATFORM "Linux")
    add_compile_definitions(DEMOPH_PLATFORM_LINUX)
endif()

# Compiler-specific settings
if(MSVC)
    add_compile_options(
        /W4 /WX /permissive- /Zc:__cplusplus /Zc:preprocessor
        /wd4251 /wd4275  # DLL interface warnings
    )
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
    
    # Enable parallel compilation
    add_compile_options(/MP)
    
    # Enable function-level linking
    add_compile_options(/Gy)
    
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /Ob2 /DNDEBUG)
        add_link_options(/OPT:REF /OPT:ICF)
    endif()
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror
        -Wno-unused-parameter -Wno-missing-field-initializers
    )
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Wno-unknown-warning-option)
    endif()
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
        
        # Sanitizers
        if(DEMOPH_ENABLE_ASAN)
            add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
            add_link_options(-fsanitize=address)
        endif()
        
        if(DEMOPH_ENABLE_TSAN)
            add_compile_options(-fsanitize=thread)
            add_link_options(-fsanitize=thread)
        endif()
        
        if(DEMOPH_ENABLE_UBSAN)
            add_compile_options(-fsanitize=undefined)
            add_link_options(-fsanitize=undefined)
        endif()
    else()
        add_compile_options(-O3 -DNDEBUG -march=native)
        add_link_options(-s)  # Strip symbols
    endif()
endif()

message(STATUS "=== Demoph Engine v${PROJECT_VERSION} Configuration ===")
message(STATUS "Platform: ${DEMOPH_PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")

# Find required packages
find_package(Threads REQUIRED)

# Graphics APIs
if(DEMOPH_ENABLE_VULKAN)
    find_package(Vulkan REQUIRED)
    add_compile_definitions(DEMOPH_VULKAN_ENABLED)
    message(STATUS "Vulkan: ${Vulkan_VERSION}")
endif()

if(DEMOPH_ENABLE_OPENGL)
    find_package(OpenGL REQUIRED)
    add_compile_definitions(DEMOPH_OPENGL_ENABLED)
    message(STATUS "OpenGL found")
endif()

if(DEMOPH_ENABLE_DIRECTX12 AND WIN32)
    add_compile_definitions(DEMOPH_DIRECTX12_ENABLED)
    message(STATUS "DirectX 12 enabled")
endif()

# Window management
find_package(glfw3 REQUIRED)

# Math library
find_package(glm REQUIRED)

# Physics
find_package(Bullet QUIET)
if(Bullet_FOUND)
    message(STATUS "Bullet Physics: Found system installation")
else()
    message(STATUS "Bullet Physics: Using bundled version")
    set(BULLET_ROOT ${CMAKE_SOURCE_DIR}/external/bullet3)
    add_subdirectory(${BULLET_ROOT} EXCLUDE_FROM_ALL)
endif()

# Audio
find_package(OpenAL REQUIRED)
message(STATUS "OpenAL: Found")

# Networking
if(DEMOPH_ENABLE_NETWORKING)
    find_package(asio QUIET)
    if(NOT asio_FOUND)
        message(STATUS "Asio: Using bundled version")
    endif()
    add_compile_definitions(DEMOPH_NETWORKING_ENABLED)
endif()

# Python scripting
if(DEMOPH_ENABLE_PYTHON)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    find_package(pybind11 REQUIRED)
    add_compile_definitions(DEMOPH_PYTHON_ENABLED)
    message(STATUS "Python: ${Python3_VERSION}")
endif()

# Lua scripting
if(DEMOPH_ENABLE_LUA)
    find_package(Lua QUIET)
    if(NOT Lua_FOUND)
        message(STATUS "Lua: Using bundled version")
        set(LUA_ROOT ${CMAKE_SOURCE_DIR}/external/lua)
    endif()
    add_compile_definitions(DEMOPH_LUA_ENABLED)
endif()

# Asset loading
find_package(assimp QUIET)
if(NOT assimp_FOUND)
    message(STATUS "Assimp: Using bundled version")
    set(ASSIMP_ROOT ${CMAKE_SOURCE_DIR}/external/assimp)
    add_subdirectory(${ASSIMP_ROOT} EXCLUDE_FROM_ALL)
endif()

# Image loading
find_package(SOIL2 QUIET)
if(NOT SOIL2_FOUND)
    message(STATUS "SOIL2: Using bundled version")
    set(SOIL2_ROOT ${CMAKE_SOURCE_DIR}/external/soil2)
endif()

# Tracy profiler
if(DEMOPH_ENABLE_TRACY)
    find_package(Tracy QUIET)
    if(NOT Tracy_FOUND)
        message(STATUS "Tracy: Using bundled version")
        add_subdirectory(${CMAKE_SOURCE_DIR}/external/tracy)
    endif()
    add_compile_definitions(DEMOPH_TRACY_ENABLED TRACY_ENABLE)
endif()

# External dependencies directory
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

# Add external dependencies
if(EXISTS ${EXTERNAL_DIR}/glad)
    add_subdirectory(${EXTERNAL_DIR}/glad)
endif()

if(EXISTS ${EXTERNAL_DIR}/imgui)
    add_subdirectory(${EXTERNAL_DIR}/imgui)
endif()

if(EXISTS ${EXTERNAL_DIR}/spdlog)
    add_subdirectory(${EXTERNAL_DIR}/spdlog)
endif()

# Engine source files
set(ENGINE_CORE_SOURCES
    # Core system
    src/core/engine.cpp
    src/core/application.cpp
    src/core/scene.cpp
    src/core/entity.cpp
    src/core/component.cpp
    src/core/system.cpp
    src/core/layer.cpp
    src/core/timestep.cpp
    src/core/log.cpp
    src/core/assert.cpp
    src/core/event.cpp
    src/core/input.cpp
    src/core/window.cpp
    
    # ECS
    src/ecs/world.cpp
    src/ecs/component_manager.cpp
    src/ecs/system_manager.cpp
    src/ecs/entity_manager.cpp
    src/ecs/archetype.cpp
    
    # Math
    src/math/vec2.cpp
    src/math/vec3.cpp
    src/math/vec4.cpp
    src/math/mat3.cpp
    src/math/mat4.cpp
    src/math/quaternion.cpp
    src/math/transform.cpp
    src/math/aabb.cpp
    src/math/obb.cpp
    src/math/plane.cpp
    src/math/frustum.cpp
    
    # Memory management
    src/memory/memory_pool.cpp
    src/memory/stack_allocator.cpp
    src/memory/linear_allocator.cpp
    src/memory/free_list_allocator.cpp
)

set(ENGINE_RENDERER_SOURCES
    # Core rendering
    src/renderer/renderer.cpp
    src/renderer/renderer2d.cpp
    src/renderer/renderer3d.cpp
    src/renderer/advanced_renderer.cpp
    src/renderer/shader.cpp
    src/renderer/texture.cpp
    src/renderer/material.cpp
    src/renderer/mesh.cpp
    src/renderer/model.cpp
    src/renderer/camera.cpp
    src/renderer/framebuffer.cpp
    src/renderer/vertex_array.cpp
    src/renderer/vertex_buffer.cpp
    src/renderer/index_buffer.cpp
    src/renderer/uniform_buffer.cpp
    src/renderer/storage_buffer.cpp
    src/renderer/render_pass.cpp
    src/renderer/pipeline.cpp
    src/renderer/graphics_context.cpp
    src/renderer/command_buffer.cpp
    src/renderer/descriptor_set.cpp
    
    # Lighting
    src/renderer/lighting/light.cpp
    src/renderer/lighting/directional_light.cpp
    src/renderer/lighting/point_light.cpp
    src/renderer/lighting/spot_light.cpp
    src/renderer/lighting/area_light.cpp
    src/renderer/lighting/environment_light.cpp
    src/renderer/lighting/light_probe.cpp
    
    # Post-processing
    src/renderer/post_processing/post_processor.cpp
    src/renderer/post_processing/bloom.cpp
    src/renderer/post_processing/tone_mapping.cpp
    src/renderer/post_processing/fxaa.cpp
    src/renderer/post_processing/ssao.cpp
    src/renderer/post_processing/ssr.cpp
    src/renderer/post_processing/motion_blur.cpp
    src/renderer/post_processing/depth_of_field.cpp
)

set(ENGINE_VULKAN_SOURCES
    # Vulkan implementation
    src/renderer/vulkan/vulkan_context.cpp
    src/renderer/vulkan/vulkan_renderer.cpp
    src/renderer/vulkan/vulkan_shader.cpp
    src/renderer/vulkan/vulkan_buffer.cpp
    src/renderer/vulkan/vulkan_texture.cpp
    src/renderer/vulkan/vulkan_pipeline.cpp
    src/renderer/vulkan/vulkan_command_buffer.cpp
    src/renderer/vulkan/vulkan_descriptor_set.cpp
    src/renderer/vulkan/vulkan_render_pass.cpp
    src/renderer/vulkan/vulkan_framebuffer.cpp
    src/renderer/vulkan/vulkan_swapchain.cpp
    src/renderer/vulkan/vulkan_device.cpp
    src/renderer/vulkan/vulkan_memory.cpp
)

set(ENGINE_OPENGL_SOURCES
    # OpenGL implementation
    src/renderer/opengl/opengl_context.cpp
    src/renderer/opengl/opengl_renderer.cpp
    src/renderer/opengl/opengl_shader.cpp
    src/renderer/opengl/opengl_buffer.cpp
    src/renderer/opengl/opengl_texture.cpp
    src/renderer/opengl/opengl_framebuffer.cpp
    src/renderer/opengl/opengl_vertex_array.cpp
)

set(ENGINE_PHYSICS_SOURCES
    # Physics system
    src/physics/physics_world.cpp
    src/physics/rigid_body.cpp
    src/physics/collider.cpp
    src/physics/physics_material.cpp
    src/physics/constraint.cpp
    src/physics/raycast.cpp
    src/physics/collision_detection.cpp
    src/physics/collision_response.cpp
    src/physics/broad_phase.cpp
    src/physics/narrow_phase.cpp
)

set(ENGINE_AUDIO_SOURCES
    # Audio system
    src/audio/audio_engine.cpp
    src/audio/advanced_audio_engine.cpp
    src/audio/audio_source.cpp
    src/audio/audio_listener.cpp
    src/audio/audio_buffer.cpp
    src/audio/audio_effect.cpp
    src/audio/audio_processor.cpp
    src/audio/streaming_audio_source.cpp
    src/audio/audio_occlusion.cpp
    src/audio/reverb_system.cpp
)

set(ENGINE_ASSET_SOURCES
    # Asset management
    src/assets/asset_manager.cpp
    src/assets/asset_loader.cpp
    src/assets/mesh_loader.cpp
    src/assets/texture_loader.cpp
    src/assets/audio_loader.cpp
    src/assets/shader_loader.cpp
    src/assets/scene_loader.cpp
    src/assets/asset_database.cpp
    src/assets/asset_registry.cpp
    src/assets/hot_reload.cpp
)

set(ENGINE_SCRIPTING_SOURCES
    # Scripting system
    src/scripting/script_engine.cpp
    src/scripting/script_component.cpp
    src/scripting/script_manager.cpp
)

set(ENGINE_PYTHON_SOURCES
    # Python scripting
    src/scripting/python/python_engine.cpp
    src/scripting/python/python_component.cpp
    src/scripting/python/python_bindings.cpp
    src/scripting/python/python_module.cpp
)

set(ENGINE_ANIMATION_SOURCES
    # Animation system
    src/animation/animator.cpp
    src/animation/animation_clip.cpp
    src/animation/bone.cpp
    src/animation/skeleton.cpp
    src/animation/animation_state_machine.cpp
    src/animation/blend_tree.cpp
    src/animation/inverse_kinematics.cpp
)

set(ENGINE_PARTICLE_SOURCES
    # Particle system
    src/particles/particle_system.cpp
    src/particles/advanced_particle_system.cpp
    src/particles/particle_emitter.cpp
    src/particles/particle.cpp
    src/particles/particle_renderer.cpp
    src/particles/force_field.cpp
)

set(ENGINE_UI_SOURCES
    # UI system
    src/ui/ui_manager.cpp
    src/ui/ui_element.cpp
    src/ui/button.cpp
    src/ui/text.cpp
    src/ui/image.cpp
    src/ui/panel.cpp
    src/ui/layout.cpp
    src/ui/style.cpp
    src/ui/theme.cpp
    src/ui/input_field.cpp
    src/ui/slider.cpp
    src/ui/checkbox.cpp
)

set(ENGINE_NETWORK_SOURCES
    # Networking
    src/network/network_manager.cpp
    src/network/client.cpp
    src/network/server.cpp
    src/network/packet.cpp
    src/network/protocol.cpp
    src/network/serialization.cpp
    src/network/replication.cpp
)

set(ENGINE_UTILITY_SOURCES
    # Utilities
    src/utils/file_system.cpp
    src/utils/timer.cpp
    src/utils/math.cpp
    src/utils/string_utils.cpp
    src/utils/platform.cpp
    src/utils/memory.cpp
    src/utils/thread_pool.cpp
    src/utils/profiler.cpp
    src/utils/hash.cpp
    src/utils/uuid.cpp
    src/utils/serialization.cpp
    src/utils/compression.cpp
    src/utils/encryption.cpp
)

set(ENGINE_PLATFORM_SOURCES
    # Platform specific
    src/platform/platform.cpp
)

if(WIN32)
    list(APPEND ENGINE_PLATFORM_SOURCES
        src/platform/windows/windows_window.cpp
        src/platform/windows/windows_file_system.cpp
        src/platform/windows/windows_thread.cpp
    )
elseif(APPLE)
    list(APPEND ENGINE_PLATFORM_SOURCES
        src/platform/macos/macos_window.cpp
        src/platform/macos/macos_file_system.cpp
        src/platform/macos/macos_thread.cpp
    )
elseif(UNIX)
    list(APPEND ENGINE_PLATFORM_SOURCES
        src/platform/linux/linux_window.cpp
        src/platform/linux/linux_file_system.cpp
        src/platform/linux/linux_thread.cpp
    )
endif()

# Combine all sources
set(ENGINE_SOURCES
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_RENDERER_SOURCES}
    ${ENGINE_PHYSICS_SOURCES}
    ${ENGINE_AUDIO_SOURCES}
    ${ENGINE_ASSET_SOURCES}
    ${ENGINE_SCRIPTING_SOURCES}
    ${ENGINE_ANIMATION_SOURCES}
    ${ENGINE_PARTICLE_SOURCES}
    ${ENGINE_UI_SOURCES}
    ${ENGINE_UTILITY_SOURCES}
    ${ENGINE_PLATFORM_SOURCES}
)

# Add Vulkan sources if enabled
if(DEMOPH_ENABLE_VULKAN)
    list(APPEND ENGINE_SOURCES ${ENGINE_VULKAN_SOURCES})
endif()

# Add OpenGL sources if enabled
if(DEMOPH_ENABLE_OPENGL)
    list(APPEND ENGINE_SOURCES ${ENGINE_OPENGL_SOURCES})
endif()

# Add Python sources if enabled
if(DEMOPH_ENABLE_PYTHON)
    list(APPEND ENGINE_SOURCES ${ENGINE_PYTHON_SOURCES})
endif()

# Add networking sources if enabled
if(DEMOPH_ENABLE_NETWORKING)
    list(APPEND ENGINE_SOURCES ${ENGINE_NETWORK_SOURCES})
endif()

# Engine headers
file(GLOB_RECURSE ENGINE_HEADERS "include/demoph/*.h" "include/demoph/*.hpp")

# Create the engine library
if(DEMOPH_BUILD_SHARED)
    add_library(demoph SHARED ${ENGINE_SOURCES} ${ENGINE_HEADERS})
    target_compile_definitions(demoph PRIVATE DEMOPH_BUILD_DLL)
    target_compile_definitions(demoph PUBLIC DEMOPH_DLL)
else()
    add_library(demoph STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
endif()

# Create alias for easier linking
add_library(Demoph::Engine ALIAS demoph)

# Set target properties
set_target_properties(demoph PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "demoph"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

# Include directories
target_include_directories(demoph
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# External include directories
if(EXISTS ${EXTERNAL_DIR}/glad)
    target_include_directories(demoph PRIVATE ${EXTERNAL_DIR}/glad/include)
endif()

if(EXISTS ${EXTERNAL_DIR}/imgui)
    target_include_directories(demoph PRIVATE ${EXTERNAL_DIR}/imgui)
endif()

if(EXISTS ${EXTERNAL_DIR}/spdlog)
    target_include_directories(demoph PRIVATE ${EXTERNAL_DIR}/spdlog/include)
endif()

# Link libraries
target_link_libraries(demoph
    PUBLIC
        Threads::Threads
        glfw
        glm::glm
        OpenAL::OpenAL
)

# Link external libraries
if(TARGET glad)
    target_link_libraries(demoph PRIVATE glad)
endif()

if(TARGET imgui)
    target_link_libraries(demoph PRIVATE imgui)
endif()

if(TARGET spdlog)
    target_link_libraries(demoph PRIVATE spdlog)
endif()

# Graphics API specific linking
if(DEMOPH_ENABLE_VULKAN)
    target_link_libraries(demoph PUBLIC Vulkan::Vulkan)
endif()

if(DEMOPH_ENABLE_OPENGL)
    target_link_libraries(demoph PUBLIC OpenGL::GL)
endif()

# Physics linking
if(TARGET BulletDynamics)
    target_link_libraries(demoph PRIVATE BulletDynamics BulletCollision LinearMath)
elseif(Bullet_FOUND)
    target_link_libraries(demoph PRIVATE ${BULLET_LIBRARIES})
    target_include_directories(demoph PRIVATE ${BULLET_INCLUDE_DIRS})
endif()

# Python linking
if(DEMOPH_ENABLE_PYTHON)
    target_link_libraries(demoph PRIVATE pybind11::embed Python3::Python)
endif()

# Asset loading
if(TARGET assimp)
    target_link_libraries(demoph PRIVATE assimp)
endif()

# Tracy profiler
if(DEMOPH_ENABLE_TRACY)
    target_link_libraries(demoph PRIVATE Tracy::TracyClient)
endif()

# Precompiled headers
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
    target_precompile_headers(demoph PRIVATE
        <memory>
        <string>
        <vector>
        <unordered_map>
        <functional>
        <chrono>
        <thread>
        <mutex>
        <atomic>
        <algorithm>
        <cmath>
    )
endif()

# Examples
if(DEMOPH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(DEMOPH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Tools
if(DEMOPH_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

# Benchmarks
if(DEMOPH_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Python bindings
if(DEMOPH_ENABLE_PYTHON)
    add_subdirectory(bindings/python)
endif()

# Install configuration
include(GNUInstallDirs)

install(TARGETS demoph
    EXPORT DemophEngineTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

install(DIRECTORY assets/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/demoph/assets
    OPTIONAL
)

install(DIRECTORY shaders/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/demoph/shaders
    FILES_MATCHING PATTERN "*.vert" PATTERN "*.frag" PATTERN "*.comp" PATTERN "*.geom" PATTERN "*.tesc" PATTERN "*.tese"
)

# Export configuration
install(EXPORT DemophEngineTargets
    FILE DemophEngineTargets.cmake
    NAMESPACE Demoph::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DemophEngine
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/DemophEngineConfig.cmake.in
    ${CMAKE_BINARY_DIR}/DemophEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DemophEngine
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/DemophEngineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_BINARY_DIR}/DemophEngineConfig.cmake
    ${CMAKE_BINARY_DIR}/DemophEngineConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DemophEngine
)

# CPack configuration for distribution
set(CPACK_PACKAGE_NAME "DemophEngine")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Demoph Engine - Professional 2D/3D Game Engine")
set(CPACK_PACKAGE_VENDOR "Demoph Studios")
set(CPACK_PACKAGE_CONTACT "support@demoph.com")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

# Platform-specific package configuration
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Demoph Engine ${PROJECT_VERSION}")
    set(CPACK_NSIS_PACKAGE_NAME "DemophEngine")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://demoph.com")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
elseif(UNIX)
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Demoph Studios <support@demoph.com>")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Status report
message(STATUS "")
message(STATUS "Features:")
message(STATUS "  Vulkan renderer: ${DEMOPH_ENABLE_VULKAN}")
message(STATUS "  OpenGL renderer: ${DEMOPH_ENABLE_OPENGL}")
message(STATUS "  DirectX 12 renderer: ${DEMOPH_ENABLE_DIRECTX12}")
message(STATUS "  Python scripting: ${DEMOPH_ENABLE_PYTHON}")
message(STATUS "  Lua scripting: ${DEMOPH_ENABLE_LUA}")
message(STATUS "  C# scripting: ${DEMOPH_ENABLE_CSHARP}")
message(STATUS "  Networking: ${DEMOPH_ENABLE_NETWORKING}")
message(STATUS "  VR support: ${DEMOPH_ENABLE_VR}")
message(STATUS "  Profiling: ${DEMOPH_ENABLE_PROFILING}")
message(STATUS "  Hot reload: ${DEMOPH_ENABLE_HOT_RELOAD}")
message(STATUS "  Tracy profiler: ${DEMOPH_ENABLE_TRACY}")
message(STATUS "")
message(STATUS "Build options:")
message(STATUS "  Examples: ${DEMOPH_BUILD_EXAMPLES}")
message(STATUS "  Tests: ${DEMOPH_BUILD_TESTS}")
message(STATUS "  Tools: ${DEMOPH_BUILD_TOOLS}")
message(STATUS "  Benchmarks: ${DEMOPH_BUILD_BENCHMARKS}")
message(STATUS "  Shared library: ${DEMOPH_BUILD_SHARED}")
message(STATUS "")
message(STATUS "Sanitizers:")
message(STATUS "  AddressSanitizer: ${DEMOPH_ENABLE_ASAN}")
message(STATUS "  ThreadSanitizer: ${DEMOPH_ENABLE_TSAN}")
message(STATUS "  UndefinedBehaviorSanitizer: ${DEMOPH_ENABLE_UBSAN}")
message(STATUS "=== Configuration Complete ===")
