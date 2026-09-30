#include "Renderer.hpp"

#include <Metal/Metal.hpp>

#include <spdlog/spdlog.h>
#include <vector>

#include "CubeVertex.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "ShaderLibrary.hpp"

// 8 вершин куба с разными цветами
std::vector<CubeVertex> cubeVertices = {
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // Передняя-левая-нижняя (Красная)
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},  // Передняя-правая-нижняя (Зеленая)
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},   // Передняя-правая-верхняя (Синяя)
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}},  // ...
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 0.0f, 1.0f}}};

// Ииндексы для 12 треугольников
std::vector<uint16_t> cubeIndices = {
    0, 1, 2, 2, 3, 0, // Передняя
    1, 5, 6, 6, 2, 1, // Правая
    5, 4, 7, 7, 6, 5, // Задняя
    4, 0, 3, 3, 7, 4, // Левая
    3, 2, 6, 6, 7, 3, // Верхняя
    4, 5, 1, 1, 0, 4  // Нижняя
};

Renderer::Renderer(const GraphicsContext &context, MetalMesh &treeMesh, MTL::Texture *treeTexture)
    : _context(context),
      _library(context.getDevice()),
      _cubePipeline(
          context.getDevice(),
          _library,
          "vertexMain",
          "fragmentMain",
          CubeVertex::getDescriptor(),
          MTL::PixelFormatBGRA8Unorm),
      _treeMesh(treeMesh),
      _treePipeline(
          context.getDevice(),
          _library,
          "vertexMesh",
          "fragmentMesh",
          getVertexDescriptor(),
          MTL::PixelFormatBGRA8Unorm),
      _treeTexture(treeTexture)
{
}

Renderer::~Renderer()
{
}

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
    encoder->setRenderPipelineState(_treePipeline.getNative());

    MeshRenderer meshRenderer;
    std::unordered_map<std::string, MTL::Texture *> treeTextures = {
        {"wood", _treeTexture},
        {"foliage", _treeTexture}};
    meshRenderer.DrawMesh(encoder, _treeMesh, treeTextures);

    /*
    encoder->setRenderPipelineState(_cubePipeline.getNative());

    MTL::Buffer *vertexBuffer = _context.getDevice()->newBuffer(
        cubeVertices.data(),
        cubeVertices.size() * sizeof(CubeVertex),
        MTL::ResourceStorageModeShared);

    MTL::Buffer *indexBuffer = _context.getDevice()->newBuffer(
        cubeIndices.data(),
        cubeIndices.size() * sizeof(uint16_t),
        MTL::ResourceStorageModeShared);

    encoder->setVertexBuffer(vertexBuffer, 0, 0);
    encoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        static_cast<NS::UInteger>(cubeIndices.size()), // Сколько индексов отрисовать (для куба — 36)
        MTL::IndexTypeUInt16,                          // Тип индекса (uint16_t)
        indexBuffer,                                   // Буфер с индексами
        0                                              // Смещение от начала буфера
    );
    */
    encoder->endEncoding();

    // 5. Отправляем кадр на экран монитора
    commandBuffer->presentDrawable(drawable);

    commandBuffer->commit(); // Видеокарта, погнали!

    // Освобождаем временные дескрипторы кадра (управление памятью)
    // vertexBuffer->release();
    // indexBuffer->release();
    renderPassDesc->release();
}