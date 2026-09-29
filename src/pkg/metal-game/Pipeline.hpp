#pragma once

#include <string>

#include <Metal/Metal.hpp>

#include "ShaderLibrary.hpp"

class Pipeline
{
public:
    Pipeline(MTL::Device *device,
             ShaderLibrary &shaderLib,
             const std::string &vertexFunc,
             const std::string &fragmentFunc,
             MTL::VertexDescriptor *vertexDesc,
             MTL::PixelFormat pixelFormat);

    ~Pipeline();

    MTL::RenderPipelineState *getNative() const;

private:
    MTL::RenderPipelineState *_pipelineState = nullptr;
};