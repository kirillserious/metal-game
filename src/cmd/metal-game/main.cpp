#include <exception>
#include <filesystem>
#include <mach-o/dyld.h>

#include <Metal/Metal.hpp>

#include <spdlog/spdlog.h>

#include "AssetsManifest.hpp"
#include "GraphicsContext.hpp"
#include "Mesh.hpp"
#include "MeshBuilder.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "mesh-file/MeshLoader.hpp"

namespace fs = std::filesystem;

// Функция, которая находит абсолютный путь к папке, где лежит исполняемый файл
fs::path get_executable_dir()
{
    uint32_t buffer_size = 0;

    // Первый вызов с нулевым размером, чтобы узнать реальный размер пути
    _NSGetExecutablePath(nullptr, &buffer_size);

    std::vector<char> buffer(buffer_size);
    if (_NSGetExecutablePath(buffer.data(), &buffer_size) == 0)
    {
        // Получаем каноничный (абсолютный) путь к бинарнику, разрешая все симлинки
        fs::path exe_path = fs::weakly_canonical(fs::path(buffer.data()));
        return exe_path.parent_path(); // Возвращаем только папку бинарника (т.е. папку build)
    }
    spdlog::error("[get_executable_dir] failed to resolve the program directory");
    throw std::runtime_error("failed to resolve the program directory");
}
struct KTXHeader10
{
    uint8_t identifier[12]; // Должен быть равен определенной строке Khronos
    uint32_t endianness;    // 0x04030201 значит правильный порядок байт
    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat; // Это нам нужно для маппинга формата
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;  // Ширина текстуры
    uint32_t pixelHeight; // Высота текстуры
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels; // Сколько мип-уровней внутри
    uint32_t bytesOfKeyValueData;  // Размер метаданных, которые нужно пропустить
};

// Функция маппинга формата OpenGL/KTX -> Metal
MTL::PixelFormat mapGlFormatToMetal(uint32_t glInternalFormat)
{
    switch (glInternalFormat)
    {
    case 0x1908:
        return MTL::PixelFormatRGBA8Unorm; // GL_RGBA8
    case 0x8C43:
        return MTL::PixelFormatRGBA8Unorm_sRGB; // GL_SRGB8_ALPHA8
    case 0x93B0:
        return MTL::PixelFormatASTC_4x4_sRGB; // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    case 0x8E8F:
        return MTL::PixelFormatBC7_RGBAUnorm_sRGB; // GL_COMPRESSED_SRGB8_ALPHA8_BC7_ARB
    default:
        return MTL::PixelFormatInvalid;
    }
}

MTL::Texture *loadKTX10WithoutLibs(const char *filePath, MTL::Device *device)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        printf("Не удалось открыть файл: %s\n", filePath);
        return nullptr;
    }

    // 1. Читаем заголовок
    KTXHeader10 header;
    file.read(reinterpret_cast<char *>(&header), sizeof(KTXHeader10));

    // Простая проверка, что это KTX 1.0
    if (header.identifier[1] != 'K' || header.identifier[2] != 'T' || header.identifier[3] != 'X')
    {
        printf("Файл не является валидным KTX 1.0\n");
        return nullptr;
    }

    // 2. Пропускаем блок метаданных (Key-Value), если он есть
    if (header.bytesOfKeyValueData > 0)
    {
        file.seekg(header.bytesOfKeyValueData, std::ios::cur);
    }

    // 3. Определяем формат и параметры
    MTL::PixelFormat pixelFormat = mapGlFormatToMetal(header.glInternalFormat);
    if (pixelFormat == MTL::PixelFormatInvalid)
    {
        printf("Формат GL 0x%X не поддерживается в этом примере\n", header.glInternalFormat);
        return nullptr;
    }

    uint32_t mipLevels = std::max(1u, header.numberOfMipmapLevels);

    // 4. Создаем текстуру в Metal
    MTL::TextureDescriptor *desc = MTL::TextureDescriptor::texture2DDescriptor(
        pixelFormat, header.pixelWidth, header.pixelHeight, mipLevels > 1);
    desc->setMipmapLevelCount(mipLevels);
    desc->setUsage(MTL::TextureUsageShaderRead);

    MTL::Texture *metalTexture = device->newTexture(desc);
    desc->release();

    // 5. Построчно читаем мип-уровни из файла напрямую в Metal
    uint32_t currentWidth = header.pixelWidth;
    uint32_t currentHeight = header.pixelHeight;

    for (uint32_t level = 0; level < mipLevels; ++level)
    {
        // В KTX перед каждым MIP-уровнем записан его размер в байтах (4 байта)
        uint32_t imageSize = 0;
        file.read(reinterpret_cast<char *>(&imageSize), sizeof(uint32_t));

        // Выделяем временный буфер под этот мип-уровень и читаем данные
        std::vector<uint8_t> mipData(imageSize);
        file.read(reinterpret_cast<char *>(mipData.data()), imageSize);

        // Рассчитываем bytesPerRow
        size_t bytesPerRow = 0;

        // ВАЖНО: Если формат сжатый (например, ASTC или BC7), шаг строки рассчитывается по блокам
        if (header.glInternalFormat == 0x93B0 || header.glInternalFormat == 0x8E8F)
        {
            // Для блоков 4x4 (BC7 и ASTC 4x4) размер блока 16 байт
            uint32_t blocksWide = (currentWidth + 3) / 4;
            bytesPerRow = blocksWide * 16;
        }
        else
        {
            // Для несжатого RGBA8
            bytesPerRow = currentWidth * 4;
        }

        // Заливаем данные в Metal
        MTL::Region region(0, 0, currentWidth, currentHeight);
        metalTexture->replaceRegion(region, level, mipData.data(), bytesPerRow);

        // В конце каждого мип-уровня в KTX может быть выравнивание (от 1 до 3 байт пустышек)
        // Считаем, сколько байт нужно пропустить, чтобы выровняться по 4 байтам
        uint32_t padding = (3 - ((imageSize + 3) % 4));
        if (padding > 0)
        {
            file.seekg(padding, std::ios::cur);
        }

        // Корректируем размеры для следующего уровня
        currentWidth = std::max(1u, currentWidth / 2);
        currentHeight = std::max(1u, currentHeight / 2);
    }

    file.close();
    return metalTexture;
}

int main()
{
    spdlog::info("Запуск игры...");

    auto selfPath = get_executable_dir();
    spdlog::info(std::string(selfPath));
    auto assetsManifest = loadAssetsManifest(selfPath / "assets");

    auto firstTexture = assetsManifest.textures[0];

    fs::path firstTreeFile = selfPath / "assets" / assetsManifest.meshes[0].path;
    fs::path firstTreeTextureFile = selfPath / "assets" / assetsManifest.textures[0].path;

    try
    {
        Window window(1024, 768, "My Metal Engine");
        MeshFile::MeshLoader meshLoader;

        GraphicsContext context(window.getNativeHandle());

        MetalMesh treeMesh;
        MeshBuilder meshBuilder(context.getDevice(), treeMesh);
        MTL::Texture *treeTexture = loadKTX10WithoutLibs(firstTreeTextureFile.c_str(), context.getDevice());
        if (treeTexture == nullptr)
        {
            throw std::runtime_error("texture is empty");
        }

        meshLoader.load(firstTreeFile, meshBuilder);

        Renderer renderer(context, treeMesh, treeTexture);

        while (!window.shouldClose())
        {
            window.pollEvents();
            renderer.drawFrame();
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Критическое исключение: {}", e.what());
        return -1;
    }

    spdlog::info("Игра успешно закрыта.");
    return 0;
}
