cmake_minimum_required(VERSION 3.20)

project(MetalGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(shaders/CompileShaders.cmake)

find_package(argparse CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# Apple libraries

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

# UFBX library

add_library(ufbx_lib STATIC 
    third_party/ufbx/ufbx.c
    third_party/ufbx/ufbx.h
)

target_include_directories(ufbx_lib PUBLIC 
    third_party/ufbx
)

# mesh-file library

add_subdirectory(src/pkg/mesh-file)

# metal-game library

add_library(metal_game_pkg STATIC
    src/pkg/metal-game/AssetsManifest.hpp
    src/pkg/metal-game/CubeVertex.hpp
    src/pkg/metal-game/GraphicsContext.cpp
    src/pkg/metal-game/GraphicsContext.hpp
    src/pkg/metal-game/Mesh.hpp
    src/pkg/metal-game/MeshBuilder.hpp
    src/pkg/metal-game/Pipeline.cpp
    src/pkg/metal-game/Pipeline.hpp
    src/pkg/metal-game/Renderer.cpp
    src/pkg/metal-game/Renderer.hpp
    src/pkg/metal-game/ShaderLibrary.cpp
    src/pkg/metal-game/ShaderLibrary.hpp
    src/pkg/metal-game/VertexLayout.cpp
    src/pkg/metal-game/VertexLayout.hpp
    src/pkg/metal-game/Window.cpp
    src/pkg/metal-game/Window.hpp

    
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
    yaml-cpp::yaml-cpp
    ${APPLE_FRAMEWORKS}
    mesh-file-pkg
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
    mesh-file-pkg
    metal_game_pkg
)

add_custom_command(TARGET metal-game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:metal-game>/assets"
)

# fbx baker cmd

add_executable(fbx-baker
    src/cmd/fbx-baker/main.cpp
) 

target_link_libraries(fbx-baker PRIVATE
    argparse::argparse
    spdlog::spdlog
    ufbx_lib
    mesh-file-pkg
)

# fbx inspector cmd

add_executable(fbx-inspector
    src/cmd/fbx-inspector/main.cpp
) 

target_link_libraries(fbx-inspector PRIVATE
    argparse::argparse
    spdlog::spdlog
    ufbx_lib
)