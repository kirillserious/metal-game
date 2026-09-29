#pragma once
#include <Metal/Metal.hpp>

class VertexLayout
{
public:
    VertexLayout();

    VertexLayout &
    addAttribute(uint32_t attribIndex, MTL::VertexFormat format, uint32_t size, uint32_t offset);

    VertexLayout &
    setStride(uint32_t stride);

    MTL::VertexDescriptor *
    build();

private:
    MTL::VertexDescriptor *_descriptor = nullptr;
};