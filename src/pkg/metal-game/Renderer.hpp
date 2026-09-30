#pragma once

#include "GraphicsContext.hpp"
#include "Mesh.hpp"
#include "Pipeline.hpp"
#include "ShaderLibrary.hpp"

class Renderer
{
public:
    Renderer(const GraphicsContext &context, MetalMesh &treeMesh, MTL::Texture *treeTexture);
    ~Renderer();

    void drawFrame();

private:
    const GraphicsContext &_context;
    ShaderLibrary _library;
    Pipeline _cubePipeline;

    MetalMesh _treeMesh;
    Pipeline _treePipeline;
    MTL::Texture *_treeTexture;
};