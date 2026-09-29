#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

// Нам нужен AppKit, чтобы дотянуться до нативного окна Mac
#define GLFW_EXPOSE_NATIVE_COCOA

#include "graphics_context.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

GraphicsContext::GraphicsContext(void *nativeWindowHandle)
    : _device(nullptr), _commandQueue(nullptr), _metalLayer(nullptr)
{
    _device = MTL::CreateSystemDefaultDevice();
    if (!_device)
    {
        throw std::runtime_error("Metal не поддерживается на этом устройстве!");
    }

    spdlog::info("Metal успешно инициализирован на GPU: {}", _device->name()->utf8String());

    _commandQueue = _device->newCommandQueue();

    // Натягиваем MetalLayer на нативное окно Cocoa
    id nativeWindow = (id)nativeWindowHandle;
    id contentView = ((id (*)(id, SEL))objc_msgSend)(nativeWindow, sel_registerName("contentView"));

    _metalLayer = CA::MetalLayer::layer();
    _metalLayer->setDevice(_device);
    _metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

    ((void (*)(id, SEL, id))objc_msgSend)(contentView, sel_registerName("setLayer:"), (id)_metalLayer);
    ((void (*)(id, SEL, bool))objc_msgSend)(contentView, sel_registerName("setWantsLayer:"), true);

    // Защищаем системные объекты от автоматического удаления
    _device->retain();
    _commandQueue->retain();
    _metalLayer->retain();
}

GraphicsContext::~GraphicsContext()
{
    if (_metalLayer)
        _metalLayer->release();
    if (_commandQueue)
        _commandQueue->release();
    if (_device)
        _device->release();

    spdlog::info("[Context] Графический контекст уничтожен.");
}