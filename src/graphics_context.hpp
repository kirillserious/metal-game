#pragma once
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

class GraphicsContext
{
public:
    GraphicsContext(void *nativeWindowHandle);
    ~GraphicsContext();

    MTL::Device *getDevice() const { return _device; }
    MTL::CommandQueue *getCommandQueue() const { return _commandQueue; }
    CA::MetalLayer *getMetalLayer() const { return _metalLayer; }

private:
    MTL::Device *_device;
    MTL::CommandQueue *_commandQueue;
    CA::MetalLayer *_metalLayer;
};