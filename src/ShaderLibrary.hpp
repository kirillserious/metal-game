#pragma once

#include <Metal/Metal.hpp>
#include <unordered_map>
#include <string>

class ShaderLibrary
{
public:
    ShaderLibrary(MTL::Device *device, const std::string &libPath = "");

    ~ShaderLibrary();

    MTL::Function *getFunction(const std::string &name);

private:
    MTL::Library *_library = nullptr;
    std::unordered_map<std::string, MTL::Function *> _functionCache;
};