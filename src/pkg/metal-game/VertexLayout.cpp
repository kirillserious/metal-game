#include "VertexLayout.hpp"

#include <Metal/Metal.hpp>

VertexLayout::VertexLayout()
{
    _descriptor = MTL::VertexDescriptor::vertexDescriptor();
}

// Метод добавления атрибута. Возвращает ссылку на себя для цепочки вызовов (Fluent API)
VertexLayout &
VertexLayout::addAttribute(uint32_t attribIndex, MTL::VertexFormat format, uint32_t offset)
{
    auto attr = _descriptor->attributes()->object(attribIndex);
    attr->setFormat(format);
    attr->setOffset(offset);
    attr->setBufferIndex(0); // Используем 0-й слот буфера по умолчанию
    return *this;
}

// Метод установки общего шага (размера) всей структуры вершины
VertexLayout &
VertexLayout::setStride(uint32_t stride)
{
    _descriptor->layouts()->object(0)->setStride(stride);
    return *this;
}

// Возвращает готовый дескриптор Metal
MTL::VertexDescriptor *
VertexLayout::build()
{
    return _descriptor;
}