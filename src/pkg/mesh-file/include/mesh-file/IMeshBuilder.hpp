#pragma once

#include <cstdint>
#include <string>
#include "FileFormat.hpp"

namespace MeshFile
{

    class IMeshBuilder
    {
    public:
        virtual ~IMeshBuilder() = default;

        // Вызывается в самом начале, когда прочитан заголовок
        virtual void Allocate(uint32_t vertexCount, uint32_t indexCount, uint32_t submeshCount) = 0;

        // Вызывается, когда прочитана таблица материалов
        virtual void AddSubmesh(const SubmeshInfo &submesh) = 0;

        // Вызывается для передачи сырых байт вершин
        virtual void WriteVertexData(const void *data, size_t sizeInBytes) = 0;

        // Вызывается для передачи сырых байт индексов
        virtual void WriteIndexData(const void *data, size_t sizeInBytes) = 0;

        // Вызывается, когда загрузка полностью завершена
        virtual void Finalize() = 0;
    };

}