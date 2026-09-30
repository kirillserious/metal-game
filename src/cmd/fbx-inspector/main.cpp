#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include "ufbx.h"

void inspect_fbx_file(const char *filepath)
{
    ufbx_error error;
    ufbx_scene *scene = ufbx_load_file(filepath, nullptr, &error);

    if (!scene)
    {
        std::cerr << "[FBX Error] Не удалось открыть файл: " << error.description.data << std::endl;
        return;
    }

    std::cout << "=== ИНСПЕКТОР FBX: " << filepath << " ===" << std::endl;

    // 1. Смотрим, сколько в файле отдельных 3D-сеток (мешей)
    std::cout << "Всего мешей (геометрии): " << scene->meshes.count << std::endl;
    for (size_t i = 0; i < scene->meshes.count; ++i)
    {
        ufbx_mesh *mesh = scene->meshes.data[i];
        std::cout << "  - Меш [" << i << "]: " << mesh->name.data
                  << " (Вершин: " << mesh->num_vertices
                  << ", Треугольников: " << mesh->num_triangles << ")" << std::endl;
    }

    // 2. Смотрим, какие материалы зашиты внутрь
    std::cout << "\nВсего материалов: " << scene->materials.count << std::endl;
    for (size_t i = 0; i < scene->materials.count; ++i)
    {
        ufbx_material *mat = scene->materials.data[i];
        std::cout << "  - Материал [" << i << "]: " << mat->name.data << std::endl;

        // Проверим, какие текстуры привязаны к материалу (если они запечены в FBX)
        for (size_t j = 0; j < mat->textures.count; ++j)
        {
            ufbx_material_texture tex = mat->textures.data[j];
            std::cout << "    * Текстура: " << tex.texture->name.data
                      << " (Файл на диске: " << tex.texture->filename.data << ")" << std::endl;
        }
    }

    // 3. Проверяем, есть ли анимация (например, покачивание веток)
    if (scene->anim_stacks.count > 0)
    {
        std::cout << "\nОбнаружена анимация! Количество треков: " << scene->anim_stacks.count << std::endl;
        for (size_t i = 0; i < scene->anim_stacks.count; ++i)
        {
            std::cout << "  - Анимация [" << i << "]: " << scene->anim_stacks.data[i]->name.data << std::endl;
        }
    }
    else
    {
        std::cout << "\nАнимаций в файле нет (статичный объект)." << std::endl;
    }

    std::cout << "==========================================\n"
              << std::endl;

    ufbx_free_scene(scene);
}

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("FBX Analyzer");

    program.add_argument("filepath");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        spdlog::error(err.what());
        return 1;
    }

    auto filepath = program.get("filepath");
    inspect_fbx_file(filepath.c_str());

    return 0;
}