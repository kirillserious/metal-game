#include "mesh-file/MeshSerializer.hpp"

#include <cstdint>
#include <fstream>

#include <spdlog/spdlog.h>

namespace MeshFile
{

    bool MeshSerializer::SerializeToDisk(const std::string &outputPath,
                                         const std::vector<Vertex> &vertices,
                                         const std::vector<uint32_t> &indices,
                                         const std::vector<SubmeshInfo> &submeshes) const
    {
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile.is_open())
        {
            spdlog::error("[MeshSerializer] Failed to open file for writing: {}", outputPath);
            return false;
        }

        // Заполняем заголовок на основе размеров векторов
        MeshHeader header;
        header.submeshCount = static_cast<uint32_t>(submeshes.size());
        header.vertexCount = static_cast<uint32_t>(vertices.size());
        header.indexCount = static_cast<uint32_t>(indices.size());

        // Последовательно выплевываем блоки в бинарный поток
        outFile.write(reinterpret_cast<const char *>(&header), sizeof(MeshHeader));
        outFile.write(reinterpret_cast<const char *>(submeshes.data()), submeshes.size() * sizeof(SubmeshInfo));
        outFile.write(reinterpret_cast<const char *>(vertices.data()), vertices.size() * sizeof(Vertex));
        outFile.write(reinterpret_cast<const char *>(indices.data()), indices.size() * sizeof(uint32_t));

        outFile.close();
        return true;
    }

}