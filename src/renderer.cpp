#include "renderer.hpp"
#include <spdlog/spdlog.h>

Renderer::Renderer(const GraphicsContext &context) : _context(context) {}

Renderer::~Renderer() {}

void Renderer::drawFrame()
{
    // 1. Запрашиваем у Metal текущую текстуру экрана («drawable»), куда можно рисовать
    CA::MetalDrawable *drawable = _context.getMetalLayer()->nextDrawable();
    if (!drawable)
        return;

    // 2. Создаем буфер команд для этого кадра
    MTL::CommandBuffer *commandBuffer = _context.getCommandQueue()->commandBuffer();

    // 3. Настраиваем проход рендеринга (Render Pass)
    MTL::RenderPassDescriptor *renderPassDesc = MTL::RenderPassDescriptor::renderPassDescriptor();
    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPassDesc->colorAttachments()->object(0);

    colorAttachment->setTexture(drawable->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear); // Говорим: "Очисти экран перед рисованием"

    // Задаем цвет очистки экрана (RGBA). Давай сделаем приятный темный Teal/Slate
    colorAttachment->setClearColor(MTL::ClearColor(0.09, 0.13, 0.22, 1.0));
    colorAttachment->setStoreAction(MTL::StoreActionStore); // Сохранить результат в текстуру кадра

    // 4. Создаем энкодер команд. Пока мы ничего не рисуем (нет 3D моделей),
    // поэтому мы его просто создаем и сразу закрываем. Сама очистка произойдет автоматически!
    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDesc);
    encoder->endEncoding();

    // 5. Отправляем кадр на экран монитора
    commandBuffer->presentDrawable(drawable);
    commandBuffer->commit(); // Видеокарта, погнали!

    // Освобождаем временные дескрипторы кадра (управление памятью)
    renderPassDesc->release();
}