#pragma once

#include "graphics_context.hpp"

class Renderer
{
public:
    Renderer(const GraphicsContext &context);
    ~Renderer();

    void drawFrame();

private:
    const GraphicsContext &_context;
};