#include <exception>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>
#include "graphics_context.hpp"
#include "renderer.hpp"
#include "window.hpp"

int main()
{
    spdlog::info("Запуск игры...");

    try
    {
        Window window(1024, 768, "My Metal Engine");
        GraphicsContext context(window.getNativeHandle());
        Renderer renderer(context);

        while (!window.shouldClose())
        {
            window.pollEvents();
            renderer.drawFrame();
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Критическое исключение: {}", e.what());
        return -1;
    }

    spdlog::info("Игра успешно закрыта.");
    return 0;
}
