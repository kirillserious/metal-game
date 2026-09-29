#include "Window.hpp"
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <iostream>

Window::Window(int width, int height, const std::string &title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Не удалось инициализировать GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Отключаем OpenGL

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!_window)
    {
        glfwTerminate();
        throw std::runtime_error("Не удалось создать окно GLFW");
    }
}

Window::~Window()
{
    if (_window)
    {
        glfwDestroyWindow(_window);
    }
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(_window);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void *Window::getNativeHandle() const
{
    // Возвращает NSWindow* как void*, чтобы не тащить Objective-C типы в другие файлы
    return glfwGetCocoaWindow(_window);
}