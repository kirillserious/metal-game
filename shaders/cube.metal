#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
};

struct VertexOutput {
    float4 position [[position]];
    float4 color;
};

// Простая структура для передачи матрицы вращения из C++
struct Uniforms {
    float4x4 rotationMatrix;
};

vertex VertexOutput vertexMain(VertexInput in [[stage_in]],
                               constant Uniforms& uniforms [[buffer(1)]]) {
    VertexOutput out;

// Временно сдвигаем куб по Z на 0.5 вперед, чтобы он попал в диапазон [0, 1]
    float3 pos = in.position;
    pos.z += 0.5f; 

    out.position = float4(pos, 1.0);

    // Умножаем позицию вершины на матрицу вращения
    //out.position = uniforms.rotationMatrix * float4(in.position, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 fragmentMain(VertexOutput in [[stage_in]]) {
    return in.color; // Просто выводим интерполированный цвет
}