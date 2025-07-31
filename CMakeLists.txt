
cmake_minimum_required(VERSION 3.16)
project(DemophEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration options
option(DEMOPH_BUILD_SHARED "Build shared library" ON)
option(DEMOPH_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(DEMOPH_BUILD_WEB "Build WebAssembly version" OFF)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# External dependencies
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

# GLAD
add_subdirectory(${EXTERNAL_DIR}/glad)

# OpenAL for audio
find_package(OpenAL REQUIRED)

# Create the main engine library
set(ENGINE_SOURCES
    src/core/engine.cpp
    src/core/entity.cpp
    src/renderer/renderer.cpp
    src/renderer/shader.cpp
    src/physics/physics_engine.cpp
    src/audio/audio_engine.cpp
    src/assets/asset_manager.cpp
)

set(ENGINE_HEADERS
    src/core/engine.hpp
    src/core/entity.hpp
    src/core/system.hpp
    src/components/transform.hpp
    src/components/mesh.hpp
    src/components/camera.hpp
    src/renderer/renderer.hpp
    src/renderer/shader.hpp
    src/physics/physics_engine.hpp
    src/audio/audio_engine.hpp
    src/assets/asset_manager.hpp
)

if(DEMOPH_BUILD_SHARED)
    add_library(demoph SHARED ${ENGINE_SOURCES} ${ENGINE_HEADERS})
else()
    add_library(demoph STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
endif()

# Include directories
target_include_directories(demoph PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/glad/include
)

# Link libraries
target_link_libraries(demoph PUBLIC
    OpenGL::GL
    glfw
    glm::glm
    glad
    OpenAL::OpenAL
)

# Compiler-specific options
if(MSVC)
    target_compile_options(demoph PRIVATE /W4)
else()
    target_compile_options(demoph PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Create executable for testing
add_executable(demoph_test test/main.cpp)
target_link_libraries(demoph_test demoph)

# Python bindings
if(DEMOPH_BUILD_PYTHON_BINDINGS)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    
    add_library(demoph_python SHARED
        bindings/python/python_bindings.cpp
    )
    
    target_link_libraries(demoph_python
        demoph
        Python3::Python
    )
    
    set_target_properties(demoph_python PROPERTIES
        PREFIX ""
        SUFFIX ".so"
    )
endif()

# WebAssembly build
if(DEMOPH_BUILD_WEB)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    
    add_executable(demoph_web
        web/main.cpp
    )
    
    target_link_libraries(demoph_web demoph)
    
    set_target_properties(demoph_web PROPERTIES
        LINK_FLAGS "-s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 --shell-file ${CMAKE_SOURCE_DIR}/web/shell.html"
    )
endif()

# Install targets
install(TARGETS demoph
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/demoph
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY shaders/
    DESTINATION share/demoph/shaders
    FILES_MATCHING PATTERN "*.vert" PATTERN "*.frag" PATTERN "*.comp"
)

# CPack configuration
set(CPACK_PACKAGE_NAME "DemophEngine")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Demoph Engine - Where Precision Meets Possibility")
set(CPACK_PACKAGE_VENDOR "Demoph Studios")

include(CPack)
