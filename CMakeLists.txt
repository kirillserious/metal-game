cmake_minimum_required(VERSION 3.20)

project(MetalGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/graphics_context.cpp
    src/graphics_context.hpp
    src/renderer.cpp
    src/renderer.hpp
    src/window.cpp
    src/window.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/third_party/metal-cpp
)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_library(FRAMEWORK_METAL Metal REQUIRED)
find_library(FRAMEWORK_FOUNDATION Foundation REQUIRED)
find_library(FRAMEWORK_QUARTZCORE QuartzCore REQUIRED)
find_library(FRAMEWORK_APPKIT AppKit REQUIRED)
find_library(FRAMEWORK_COCOA Cocoa REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${FRAMEWORK_METAL}
    ${FRAMEWORK_FOUNDATION}
    ${FRAMEWORK_QUARTZCORE}
    ${FRAMEWORK_APPKIT}
    ${FRAMEWORK_COCOA}
)

configure_file(shaders/Shaders.metal Shaders.metal COPYONLY)