#include "ShaderLibrary.hpp"

#include <Metal/Metal.hpp>
#include <unordered_map>
#include <string>

ShaderLibrary::ShaderLibrary(MTL::Device *device, const std::string &libPath)
{
    if (libPath.empty())
    {
        // Загружаем дефолтную библиотеку из ресурсов
        _library = device->newDefaultLibrary();
    }
    else
    {
        // Альтернативный вариант: загрузка конкретного файла по пути
        auto pathStr = NS::String::string(libPath.c_str(), NS::UTF8StringEncoding);
        NS::Error *error = nullptr;
        _library = device->newLibrary(pathStr, &error);
    }

    if (!_library)
    {
        throw std::runtime_error("Failed to load Metal library!");
    }
}

ShaderLibrary::~ShaderLibrary()
{
    for (auto &[name, func] : _functionCache)
    {
        func->release();
    }
    _library->release();
}

// Возвращает функцию (из кэша или загружает заново)
MTL::Function *
ShaderLibrary::getFunction(const std::string &name)
{
    if (_functionCache.find(name) != _functionCache.end())
    {
        return _functionCache[name];
    }

    auto nsName = NS::String::string(name.c_str(), NS::UTF8StringEncoding);
    MTL::Function *func = _library->newFunction(nsName);
    if (!func)
    {
        throw std::runtime_error("Shader function not found: " + name);
    }

    _functionCache[name] = func;
    return func;
}