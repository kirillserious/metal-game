#pragma once

#include <Metal/Metal.hpp>
#include <simd/simd.h>

#include "VertexLayout.hpp"

struct CubeVertex
{
    simd_float3 position;
    simd_float4 color;

    static MTL::VertexDescriptor *getDescriptor()
    {
        return VertexLayout()
            .addAttribute(0, MTL::VertexFormatFloat3, sizeof(position), offsetof(CubeVertex, position))
            .addAttribute(1, MTL::VertexFormatFloat4, sizeof(color), offsetof(CubeVertex, color))
            .setStride(sizeof(CubeVertex))
            .build();
    }
};