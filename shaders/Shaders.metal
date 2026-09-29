#include <metal_stdlib>
using namespace metal;

// 1. Структура данных, которая приходит ИЗ C++ кода (описывает одну вершину)
struct VertexInput {
    float3 position  [[attribute(0)]]; // Координаты X, Y, Z
    float4 color     [[attribute(1)]]; // Цвет вершины (RGBA)
};

// 2. Структура данных, которую вершинный шейдер передает во фрагментный
struct VertexOutput {
    float4 position [[position]];      // Обязательное системное поле для Metal
    float4 color;                      // Передаем цвет дальше для интерполяции
};

// 3. Вершинный шейдер (Vertex Function)
vertex VertexOutput vertexMain(VertexInput in [[stage_in]],
                               constant float4x4& modelViewProjectionMatrix [[buffer(1)]]) 
{
    VertexOutput out;
    
    // Перемножаем матрицу камеры/проекции на 3D-координату, чтобы получить 2D-точку на экране
    out.position = modelViewProjectionMatrix * float4(in.position, 1.0);
    
    // Просто пробрасываем цвет вершины дальше
    out.color = in.color;
    
    return out;
}

// 4. Фрагментный шейдер (Fragment Function)
fragment float4 fragmentMain(VertexOutput in [[stage_in]]) {
    // Возвращаем итоговый цвет пикселя. 
    // Metal сам плавно смешает цвета между вершинами треугольника!
    return in.color;
}
