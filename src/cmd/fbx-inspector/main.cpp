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

        // 1. Получаем базовый цвет (Diffuse / Base Color)
        // ufbx автоматически заполняет mat->pbr.base_color.factor, конвертируя старые материалы
        ufbx_vec3 color = mat->pbr.base_color.value_vec3;
        std::cout << "    * Цвет (RGB): ["
                  << color.x << ", "
                  << color.y << ", "
                  << color.z << "]" << std::endl;

        // 2. Альтернативный способ: чтение напрямую из свойств FBX, если автоматическая конвертация не сработала
        ufbx_prop *diffuse_prop = ufbx_find_prop(&mat->props, "DiffuseColor");
        if (diffuse_prop)
        {
            ufbx_vec3 color_raw = diffuse_prop->value_vec3;
            std::cout << "    * Исходный DiffuseColor: ["
                      << color_raw.x << ", " << color_raw.y << ", " << color_raw.z << "]" << std::endl;
        }

        // 3. Чтение параметров блика (Specular)
        ufbx_prop *specular_prop = ufbx_find_prop(&mat->props, "SpecularColor");
        if (specular_prop)
        {
            ufbx_vec3 spec = specular_prop->value_vec3;
            std::cout << "    * Блик Specular: [" << spec.x << ", " << spec.y << ", " << spec.z << "]" << std::endl;
        }

        ufbx_texture *diffuse_tex = mat->pbr.base_color.texture;
        if (diffuse_tex)
        {
            std::cout << "    * Диффузная карта: " << diffuse_tex->filename.data << std::endl;
        }

        ufbx_texture *normal_tex = mat->pbr.normal_map.texture;
        if (normal_tex)
        {
            std::cout << "    * Карта нормалей: " << normal_tex->filename.data << std::endl;
        }

        // 2. Универсальный обход ВСЕХ свойств материала
        for (size_t j = 0; j < mat->props.props.count; ++j)
        {
            ufbx_prop &prop = mat->props.props.data[j];

            // Передаем сам mat вместо &mat->element
            ufbx_texture *tex = ufbx_find_prop_texture(mat, prop.name.data);

            if (tex)
            {
                std::cout << "    * Текстура в свойстве \"" << prop.name.data << "\": "
                          << tex->filename.data << std::endl;
            }
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