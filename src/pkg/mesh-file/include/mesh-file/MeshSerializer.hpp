#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "FileFormat.hpp"

namespace MeshFile
{

    class MeshSerializer
    {
    public:
        // Конструктор может принимать настройки сжатия, если они появятся в будущем
        MeshSerializer() = default;

        // Главный метод маршалинга (сериализации)
        bool SerializeToDisk(const std::string &outputPath,
                             const std::vector<Vertex> &vertices,
                             const std::vector<uint32_t> &indices,
                             const std::vector<SubmeshInfo> &submeshes) const;
    };

}