#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <argparse/argparse.hpp>
#include "ufbx.h"

#include <mesh-file/FileFormat.hpp>

using namespace MeshFile;

bool ConvertFBXWithMaterials(const std::string &fbxPath, const std::string &meshPath)
{
    ufbx_load_opts opts = {};
    opts.target_axes = ufbx_axes_right_handed_y_up; // Система координат под Metal
    opts.target_unit_meters = 1.0f;                 // Приводим масштаб к метрам

    ufbx_error error;
    ufbx_scene *scene = ufbx_load_file(fbxPath.c_str(), &opts, &error);
    if (!scene)
    {
        std::cerr << "Failed to load FBX: " << error.description.data << std::endl;
        return false;
    }

    std::vector<Vertex> outVertices;
    std::vector<uint32_t> outIndices;
    std::vector<SubmeshInfo> outSubmeshes;

    // Для простоты и гарантии стабильности воспользуемся flat-сборкой:
    // Каждому индексу соответствует своя уникальная вершина.
    // Но собирать их мы будем строго последовательно по материалам!

    for (size_t i = 0; i < scene->meshes.count; ++i)
    {
        ufbx_mesh *fbxMesh = scene->meshes.data[i];
        if (fbxMesh->faces.count == 0)
            continue;

        size_t numMeshMaterials = fbxMesh->materials.count;
        size_t loopCount = numMeshMaterials > 0 ? numMeshMaterials : 1;

        for (size_t m = 0; m < loopCount; ++m)
        {
            ufbx_material *currentMaterial = nullptr;
            if (numMeshMaterials > 0)
                currentMaterial = fbxMesh->materials.data[m];

            SubmeshInfo submesh = {};
            // Запоминаем, с какого места в ГЛОБАЛЬНОМ массиве индексов начинается этот материал
            submesh.indexOffset = static_cast<uint32_t>(outIndices.size());

            std::string matName = currentMaterial ? currentMaterial->name.data : "default_material";
            std::strncpy(&submesh.materialName[0], matName.c_str(), sizeof(submesh.materialName) - 1);
            submesh.materialName[sizeof(submesh.materialName) - 1] = '\0';

            // Локальный счетчик добавленных вершин для ЭТОГО материала
            uint32_t submeshIndicesCount = 0;

            for (size_t fi = 0; fi < fbxMesh->faces.count; ++fi)
            {
                if (numMeshMaterials > 0 && fbxMesh->face_material.data[fi] != m)
                    continue;

                ufbx_face face = fbxMesh->faces.data[fi];
                size_t numTriangles = face.num_indices - 2;

                for (size_t ti = 0; ti < numTriangles; ++ti)
                {
                    size_t cornerIndices[3] = {
                        face.index_begin,
                        face.index_begin + ti + 1,
                        face.index_begin + ti + 2};

                    for (int c = 0; c < 3; ++c)
                    {
                        size_t meshVertexIdx = cornerIndices[c];
                        Vertex v = {};

                        // Позиция
                        ufbx_vec3 pos = ufbx_get_vertex_vec3(&fbxMesh->vertex_position, meshVertexIdx);
                        v.position[0] = (float)pos.x;
                        v.position[1] = (float)pos.y;
                        v.position[2] = (float)pos.z;

                        // Нормаль
                        if (fbxMesh->vertex_normal.exists)
                        {
                            ufbx_vec3 norm = ufbx_get_vertex_vec3(&fbxMesh->vertex_normal, meshVertexIdx);
                            v.normal[0] = (float)norm.x;
                            v.normal[1] = (float)norm.y;
                            v.normal[2] = (float)norm.z;
                        }

                        // UV
                        if (fbxMesh->vertex_uv.exists)
                        {
                            ufbx_vec2 uv = ufbx_get_vertex_vec2(&fbxMesh->vertex_uv, meshVertexIdx);
                            v.texCoords[0] = (float)uv.x;
                            v.texCoords[1] = (float)uv.y;
                        }

                        // ВАЖНО: Индекс должен указывать на глобальное положение вершины в outVertices!
                        outIndices.push_back(static_cast<uint32_t>(outVertices.size()));
                        outVertices.push_back(v);

                        submeshIndicesCount++;
                    }
                }
            }

            submesh.indexCount = submeshIndicesCount;

            if (submesh.indexCount > 0)
            {
                outSubmeshes.push_back(submesh);
            }
        }
    }

    ufbx_free_scene(scene);

    if (outVertices.empty() || outSubmeshes.empty())
    {
        std::cerr << "No valid geometry found in FBX: " << fbxPath << std::endl;
        return false;
    }

    // Запись бинарного .mesh файла
    std::ofstream outFile(meshPath, std::ios::binary);
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open output file: " << meshPath << std::endl;
        return false;
    }

    MeshHeader header;
    header.vertexCount = static_cast<uint32_t>(outVertices.size());
    header.indexCount = static_cast<uint32_t>(outIndices.size());
    header.submeshCount = static_cast<uint32_t>(outSubmeshes.size());

    // Записываем структуру последовательно
    outFile.write(reinterpret_cast<const char *>(&header), sizeof(MeshHeader));
    outFile.write(reinterpret_cast<const char *>(outSubmeshes.data()), outSubmeshes.size() * sizeof(SubmeshInfo));
    outFile.write(reinterpret_cast<const char *>(outVertices.data()), outVertices.size() * sizeof(Vertex));
    outFile.write(reinterpret_cast<const char *>(outIndices.data()), outIndices.size() * sizeof(uint32_t));

    outFile.close();

    std::cout << "Successfully baked " << meshPath << "\n"
              << "  - Submeshes (Materials): " << header.submeshCount << "\n"
              << "  - Vertices: " << header.vertexCount << "\n"
              << "  - Indices: " << header.indexCount << std::endl;

    return true;
}

int main(int argc, char *argv[])
{

    argparse::ArgumentParser program("FBX Analyzer");

    program.add_argument("src");
    program.add_argument("dst");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return ConvertFBXWithMaterials(program.get("src"), program.get("dst")) ? 0 : -1;
}
