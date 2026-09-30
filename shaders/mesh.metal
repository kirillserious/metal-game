#include <metal_stdlib>
using namespace metal;

// 1. Описываем вершину точно так же, как в C++ (32 байта)
struct Vertex {
    float3 position  [[attribute(0)]]; // Находится в маппинге под под-индексом 0
    float3 normal    [[attribute(1)]]; // Под-индекс 1
    float2 texCoords [[attribute(2)]]; // Под-индекс 2
};

struct VertexOut {
    float4 position [[position]];
    float3 normal;
    float2 texCoords;
};

struct Uniforms {
    float4x4 modelViewProjectionMatrix;
};

// 2. Вершинный шейдер (Vertex Shader)
vertex VertexOut vertexMesh(
    Vertex v [[stage_in]], // stage_in автоматически собирает Vertex на основе VertexDescriptor
    constant Uniforms& uniforms [[buffer(1)]]) 
{
    VertexOut out;
    out.position = uniforms.modelViewProjectionMatrix * float4(v.position, 1.0);
    out.normal = v.normal;
    out.texCoords = v.texCoords;

    float3 debugPos = v.position * 1; // Уменьшаем в 100 раз
    out.position = float4(debugPos, 1.0);
    return out;
}

// 3. Фрагментный шейдер (Fragment Shader)
fragment float4 fragmentMesh(
    VertexOut in [[stage_in]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]]) 
{
    // Сэмплируем текстуру (кору или листья, в зависимости от того, что забиндил Renderer)
    float4 color = diffuseTexture.sample(textureSampler, in.texCoords);
    return color;
}
