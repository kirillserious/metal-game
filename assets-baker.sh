#!/usr/bin/env bash

set -e; set -o pipefail; set -m
my_dir=$(dirname $(readlink -f "$0"))

export REPO_DIR="$my_dir"

trees_folder="${REPO_DIR}/assets-raw/models/trees"

# Создаем папки для выходных бинарников и текстур
mkdir -p "$REPO_DIR/assets/models/trees"
mkdir -p "$REPO_DIR/assets/textures"

# Находим путь к TextureConverter через xcrun один раз перед циклом
TEXTURE_CONVERTER=$(xcrun --find TextureConverter)

for file_path in "$trees_folder"/*; do
    # Проверяем, что это файл, а не пустая папка (на случай, если она пуста)
    [ -f "$file_path" ] || continue

    # Вытаскиваем расширение файла в нижнем регистре
    extension="${file_path##*.}"
    extension=$(echo "$extension" | tr '[:upper:]' '[:lower:]')

    case "$extension" in
        fbx)
            filename=$(basename "$file_path" .fbx)
            echo "Processing geometry: $filename.fbx -> $filename.bin"
            
            # Ваша Тула для геометрии
            $REPO_DIR/build/fbx-baker "$file_path" "$REPO_DIR/assets/models/trees/$filename.bin"
            ;;
            
        png)
            filename=$(basename "$file_path" .png)
            echo "Processing texture: $filename.png -> $filename.ktx2"
            
            xcrun textureconverter -f KTX -e ASTC --block-width-4 --block-height-4 -o "$REPO_DIR/assets/textures/$filename.ktx" "$file_path"

            ;;
            
        *)
            # Игнорируем файлы с другими расширениями (.DS_Store, .txt и т.д.)
            ;;
    esac
done