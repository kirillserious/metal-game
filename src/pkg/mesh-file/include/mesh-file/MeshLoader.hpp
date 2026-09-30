#pragma once

#include <fstream>
#include <string>

#include "IMeshBuilder.hpp"

namespace MeshFile
{

    class MeshLoader
    {
    public:
        MeshLoader() = default;

        // Передаем ссылку на абстрактный строитель
        bool load(const std::string &filepath, IMeshBuilder &builder) const
        {
            std::ifstream file(filepath, std::ios::binary);
            if (!file.is_open())
                return false;

            // 1. Читаем заголовок
            MeshHeader header;
            file.read(reinterpret_cast<char *>(&header), sizeof(MeshHeader));
            if (header.magic[0] != 'M' || header.magic[1] != 'E' || header.magic[2] != 'S' || header.magic[3] != 'H')
            {
                return false;
            }

            // Говорим приемнику: "Приготовься, выдели память!"
            builder.Allocate(header.vertexCount, header.indexCount, header.submeshCount);

            // 2. Читаем сабмеши по одному и отдаем в builder
            for (uint32_t i = 0; i < header.submeshCount; ++i)
            {
                SubmeshInfo submesh;
                file.read(reinterpret_cast<char *>(&submesh), sizeof(SubmeshInfo));
                builder.AddSubmesh(submesh);
            }

            // 3. Читаем вершины
            size_t vertexSize = header.vertexCount * sizeof(Vertex);
            // Если builder позволяет писать напрямую в свой буфер (например, GPU)
            // Мы передаем данные куском. Но лоадер не знает, куда они идут!

            // Временный стек-буфер для чтения (или читаем частями, если файл огромный,
            // но для дерева можно прочитать в вектор и отдать)
            std::vector<char> buffer(vertexSize);
            file.read(buffer.data(), vertexSize);
            builder.WriteVertexData(buffer.data(), vertexSize);

            // 4. Читаем индексы
            size_t indexSize = header.indexCount * sizeof(uint32_t);
            buffer.resize(indexSize);
            file.read(buffer.data(), indexSize);
            builder.WriteIndexData(buffer.data(), indexSize);

            builder.Finalize();
            file.close();
            return true;
        }
    };

}