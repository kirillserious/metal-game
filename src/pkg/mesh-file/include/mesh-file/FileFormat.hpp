#pragma once

#include <cstdint>

namespace MeshFile
{

    // 1. Заголовок — ровно 16 байт
    struct MeshHeader
    {
        char magic[4] = {'M', 'E', 'S', 'H'}; // 4 байта
        uint32_t submeshCount = 0;            // 4 байта
        uint32_t vertexCount = 0;             // 4 байта
        uint32_t indexCount = 0;              // 4 байта
    };

    // 2. Сабмеш (Материал) — ровно 72 байта
    struct SubmeshInfo
    {
        uint32_t indexOffset;        // 4 байта — с какого индекса в файле начинается кусок
        uint32_t indexCount;         // 4 байта — сколько индексов он занимает
        char materialName[64] = {0}; // 64 байта — имя материала ("wood", "foliage")
    };

    // 3. Одна Вершина — ровно 32 байта
    // Обратите внимание на alignas(16). Это нужно, чтобы Metal работал с максимальной скоростью,
    // поэтому размер структуры дополняется пустыми байтами (padding) до 32 байт.
    struct alignas(16) Vertex
    {
        float position[3];  // 12 байт (X, Y, Z)
        float normal[3];    // 12 байт (NX, NY, NZ)
        float texCoords[2]; // 8 байт (U, V)
    };
}
