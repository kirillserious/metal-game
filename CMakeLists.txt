cmake_minimum_required(VERSION 3.20)

project(MetalGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(shaders/CompileShaders.cmake)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_library(FRAMEWORK_METAL Metal REQUIRED)
find_library(FRAMEWORK_FOUNDATION Foundation REQUIRED)
find_library(FRAMEWORK_QUARTZCORE QuartzCore REQUIRED)
find_library(FRAMEWORK_APPKIT AppKit REQUIRED)
find_library(FRAMEWORK_COCOA Cocoa REQUIRED)

set(APPLE_FRAMEWORKS
    ${FRAMEWORK_METAL}
    ${FRAMEWORK_FOUNDATION}
    ${FRAMEWORK_QUARTZCORE}
    ${FRAMEWORK_APPKIT}
    ${FRAMEWORK_COCOA}
)

# metal-game library

add_library(metal_game_pkg STATIC
    src/pkg/metal-game/GraphicsContext.cpp
    src/pkg/metal-game/GraphicsContext.hpp
    src/pkg/metal-game/Renderer.cpp
    src/pkg/metal-game/Renderer.hpp
    src/pkg/metal-game/ShaderLibrary.cpp
    src/pkg/metal-game/ShaderLibrary.hpp
    src/pkg/metal-game/VertexLayout.cpp
    src/pkg/metal-game/VertexLayout.hpp
    src/pkg/metal-game/Window.cpp
    src/pkg/metal-game/Window.hpp
    src/pkg/metal-game/Pipeline.cpp
    src/pkg/metal-game/Pipeline.hpp
    src/pkg/metal-game/CubeVertex.hpp
)

target_include_directories(metal_game_pkg 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pkg/metal-game
    SYSTEM 
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/metal-cpp
)


target_link_libraries(metal_game_pkg PRIVATE
    fmt::fmt
    spdlog::spdlog
    glfw
    ${APPLE_FRAMEWORKS}
)

add_metal_shaders_directory(
    metal_game_pkg 
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders" 
    "default"
)


# metal_game_cmd

add_executable(metal-game
    src/cmd/metal-game/main.cpp
) 

target_link_libraries(metal-game PRIVATE
    spdlog::spdlog
    metal_game_pkg
)

# hello_world_cmd

add_executable(hello-world
    src/cmd/hello-world/main.cpp
) 

target_link_libraries(hello-world PRIVATE
    spdlog::spdlog
    metal_game_pkg
)