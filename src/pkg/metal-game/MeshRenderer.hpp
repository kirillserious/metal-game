#pragma once

#include <string>
#include <unordered_map>

#include <Metal/Metal.hpp>
#include "Mesh.hpp"

class MeshRenderer
{
public:
    void DrawMesh(MTL::RenderCommandEncoder *renderEncoder, const MetalMesh &mesh, const std::unordered_map<std::string, MTL::Texture *> &textures)
    {
        if (mesh.indexBuffer == nullptr)
        {
            spdlog::critical("Индексный буфер вообще не инициализирован (nullptr)!");
            return;
        }

        size_t actualBufferSize = mesh.indexBuffer->length();

        // 1. Привязываем Vertex Buffer (Буфер Вершин) в нулевой индекс (buffer(0) в шейдере)
        // Мы делаем это ОДИН раз для всего дерева, так как вся геометрия лежит в одном монолите
        renderEncoder->setVertexBuffer(mesh.vertexBuffer, 0, 0);

        // 2. Итерируемся по сабмешам (в вашем случае их будет 3)
        for (const auto &submesh : mesh.submeshes)
        {
            size_t offsetInBytes = submesh.indexOffset * sizeof(uint32_t);
            size_t totalRequiredBytes = offsetInBytes + (submesh.indexCount * sizeof(uint32_t));
            spdlog::info("Отрисовка сабмеша [{}]: оффсет элементов={},count={}, оффсет в байтах={}",
                         submesh.materialName, submesh.indexOffset, submesh.indexCount, offsetInBytes);
            spdlog::info("Требуется байт в буфере: {}, Реальный размер буфера Metal: {}",
                         totalRequiredBytes, actualBufferSize);

            if (totalRequiredBytes > actualBufferSize)
            {
                spdlog::critical("КРАШ ПРЕДОТВРАЩЕН: Попытка прочесть {} байт из буфера размером {}! Ошибка парсинга файла.",
                                 totalRequiredBytes, actualBufferSize);
                return; // Не вызываем drawIndexedPrimitives, чтобы не поймать Segfault
            }
            // === ВАЖНО: ЗДЕСЬ БУДЕТ СМЕНА МАТЕРИАЛА ===
            // Видеокарта за один вызов рисует только один материал.
            // С помощью submesh.materialName мы понимаем, какую текстуру активировать:
            // if (strcmp(submesh.materialName, "foliage") == 0) {
            //     renderEncoder->setFragmentTexture(textureLeaves, 0);
            // } else if (strcmp(submesh.materialName, "wood") == 0) {
            //     renderEncoder->setFragmentTexture(textureBark, 0);
            // }
            // =========================================

            // 3. Вызываем команду отрисовки треугольников по индексам (Draw Call)

            auto it = textures.find(submesh.materialName);
            if (it != textures.end() && it->second != nullptr)
            {
                // Привязываем текстуру к индексу 0 фрагментного шейдера: texture(0)
                renderEncoder->setFragmentTexture(it->second, 0);
            }
            else
            {
                // Если текстура не найдена, можно либо выдать предупреждение,
                // либо привязать дефолтную (заглушку) текстуру 1х1, чтобы шейдер не упал
                spdlog::warn("Текстура для материала '{}' не найдена! Будет использована прошлая текстура.", submesh.materialName);
            }

            renderEncoder->drawIndexedPrimitives(
                MTL::PrimitiveTypeTriangle,            // Рисуем чистые треугольники
                submesh.indexCount,                    // Сколько индексов занимает этот материал
                MTL::IndexTypeUInt32,                  // Тип индексов (uint32_t)
                mesh.indexBuffer,                      // Сам буфер индексов Metal
                submesh.indexOffset * sizeof(uint32_t) // Смещение в байтах до начала этого материала!
            );
            spdlog::info("after drawIndexedPrimitives");
        }
    }
};