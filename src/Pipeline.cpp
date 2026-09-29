#include "Pipeline.hpp"

#include <Metal/Metal.hpp>
#include <exception>
#include <string>

Pipeline::Pipeline(MTL::Device *device,
                   ShaderLibrary &shaderLib,
                   const std::string &vertexFunc,
                   const std::string &fragmentFunc,
                   MTL::VertexDescriptor *vertexDesc,
                   MTL::PixelFormat pixelFormat)
{
    auto desc = MTL::RenderPipelineDescriptor::alloc()->init();
    desc->setVertexFunction(shaderLib.getFunction(vertexFunc));
    desc->setFragmentFunction(shaderLib.getFunction(fragmentFunc));
    desc->setVertexDescriptor(vertexDesc);
    desc->colorAttachments()->object(0)->setPixelFormat(pixelFormat);

    NS::Error *error = nullptr;
    _pipelineState = device->newRenderPipelineState(desc, &error);
    desc->release();

    if (!_pipelineState)
    {
        throw std::runtime_error("Failed to create pipeline state!");
    }
}

Pipeline::~Pipeline()
{
    _pipelineState->release();
}

MTL::RenderPipelineState *
Pipeline::getNative() const { return _pipelineState; }