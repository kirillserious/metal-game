#pragma once

#include <Metal/Metal.hpp>
#include "mesh-file/IMeshBuilder.hpp"
#include "Mesh.hpp"

class MeshBuilder : public MeshFile::IMeshBuilder
{
public:
    MeshBuilder(MTL::Device *device, MetalMesh &outMesh)
        : _device(device), _mesh(outMesh) {}

    void Allocate(uint32_t vertexCount, uint32_t indexCount, uint32_t submeshCount) override
    {
        _mesh.totalIndices = indexCount;
        _mesh.submeshes.reserve(submeshCount);

        // Сразу выделяем память на GPU!
        spdlog::info("[MeshBuilder::Allocate] vertexCount={}, indexCount={}", vertexCount, indexCount);
        _mesh.vertexBuffer = _device->newBuffer(vertexCount * sizeof(MeshFile::Vertex), MTL::ResourceStorageModeShared);
        _mesh.indexBuffer = _device->newBuffer(indexCount * sizeof(uint32_t), MTL::ResourceStorageModeShared);
    }

    void AddSubmesh(const MeshFile::SubmeshInfo &submesh) override
    {
        _mesh.submeshes.push_back(submesh);
    }

    void WriteVertexData(const void *data, size_t sizeInBytes) override
    {
        // Копируем байты прямо в память видеокарты
        std::memcpy(_mesh.vertexBuffer->contents(), data, sizeInBytes);
    }

    void WriteIndexData(const void *data, size_t sizeInBytes) override
    {
        // Копируем байты прямо в память видеокарты
        std::memcpy(_mesh.indexBuffer->contents(), data, sizeInBytes);
    }

    void Finalize() override {}

private:
    MTL::Device *_device;
    MetalMesh &_mesh;
};
