#pragma once

#include <vector>

#include <Metal/Metal.hpp>
#include <spdlog/spdlog.h>

#include "mesh-file/FileFormat.hpp"

#include "VertexLayout.hpp"

struct MetalMesh
{
    MTL::Buffer *vertexBuffer = nullptr;
    MTL::Buffer *indexBuffer = nullptr;
    std::vector<MeshFile::SubmeshInfo> submeshes;
    uint32_t totalIndices = 0;
};

inline MTL::VertexDescriptor *
getVertexDescriptor()
{
    spdlog::info("Размер структуры {}", sizeof(MeshFile::Vertex));
    return VertexLayout()
        .addAttribute(0, MTL::VertexFormatFloat3, offsetof(MeshFile::Vertex, position))
        .addAttribute(1, MTL::VertexFormatFloat3, offsetof(MeshFile::Vertex, normal))
        .addAttribute(2, MTL::VertexFormatFloat2, offsetof(MeshFile::Vertex, texCoords))
        .setStride(sizeof(MeshFile::Vertex))
        .build();
}
