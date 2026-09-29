#pragma once

#include "GraphicsContext.hpp"
#include "Pipeline.hpp"
#include "ShaderLibrary.hpp"

class Renderer
{
public:
    Renderer(const GraphicsContext &context);
    ~Renderer();

    void drawFrame();

private:
    const GraphicsContext &_context;
    ShaderLibrary _library;
    Pipeline _cubePipeline;
};