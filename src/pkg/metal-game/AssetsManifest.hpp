#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>

struct MaterialInfo
{
    std::string name;
    std::string diffuseMap;
};

struct MeshInfo
{
    std::string id;
    std::string path;
    std::vector<MaterialInfo> materials;
};

struct TextureInfo
{
    std::string id;
    std::string path;
};

// Структура самого манифеста, в точности повторяющая твой YAML
struct AssetsManifest
{
    std::vector<MeshInfo> meshes;
    std::vector<TextureInfo> textures;
};

AssetsManifest
loadAssetsManifest(const std::filesystem::path &assetsFolder)
{
    std::filesystem::path manifestPath = assetsFolder / "scene.yaml";

    // Строгая проверка на существование до парсинга
    if (!std::filesystem::exists(manifestPath))
    {
        throw std::filesystem::filesystem_error(
            "Assets manifest missing",
            manifestPath,
            std::make_error_code(std::errc::no_such_file_or_directory));
    }

    try
    {
        YAML::Node root = YAML::LoadFile(manifestPath.string());

        return root.as<AssetsManifest>();
    }
    catch (const YAML::Exception &e)
    {
        throw;
    }
}

// Специализация yaml-cpp
namespace YAML
{
    template <>
    struct convert<TextureInfo>
    {
        static bool decode(const Node &node, TextureInfo &rhs)
        {
            if (!node.IsMap() || !node["id"] || !node["path"])
                return false;
            rhs.id = node["id"].as<std::string>();
            rhs.path = node["path"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<MaterialInfo>
    {
        static bool decode(const Node &node, MaterialInfo &rhs)
        {
            if (!node.IsMap() || !node["name"] || !node["diffuseMap"])
                return false;
            rhs.name = node["name"].as<std::string>();
            rhs.diffuseMap = node["diffuseMap"].as<std::string>();
            return true;
        }
    };

    template <>
    struct convert<MeshInfo>
    {
        static bool decode(const Node &node, MeshInfo &rhs)
        {
            if (!node.IsMap() || !node["id"] || !node["path"])
                return false;
            rhs.id = node["id"].as<std::string>();
            rhs.path = node["path"].as<std::string>();
            if (node["materials"])
            {
                rhs.materials = node["materials"].as<std::vector<MaterialInfo>>();
            }
            return true;
        }
    };

    template <>
    struct convert<AssetsManifest>
    {
        static bool decode(const Node &node, AssetsManifest &rhs)
        {
            if (!node.IsMap() || !node["assets"])
                return false;
            auto assets = node["assets"];
            if (assets["meshes"])
            {
                rhs.meshes = assets["meshes"].as<std::vector<MeshInfo>>();
            }
            if (assets["textures"])
            {
                rhs.textures = assets["textures"].as<std::vector<TextureInfo>>();
            }
            return true;
        }
    };
}
