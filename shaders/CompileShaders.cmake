function(add_metal_shaders_directory TARGET_NAME SHADERS_DIR OUTPUT_LIB_NAME)
    # Находим все .metal файлы в указанной папке
    file(GLOB SHADER_SOURCES "${SHADERS_DIR}/*.metal")
    
    # Если файлов нет, выводим предупреждение и выходим
    if(NOT SHADER_SOURCES)
        message(WARNING "No Metal shader files found in directory: ${SHADERS_DIR}")
        return()
    endif()

    set(AIR_FILES "")
    
    # Компилируем каждый шейдер в .air
    foreach(SHADER_SRC ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_SRC} NAME_WE)
        set(OUTPUT_AIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.air")
        list(APPEND AIR_FILES ${OUTPUT_AIR})
        
        add_custom_command(
            OUTPUT ${OUTPUT_AIR}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
            COMMAND xcrun -sdk macosx metal -c "${SHADER_SRC}" -o "${OUTPUT_AIR}"
            DEPENDS "${SHADER_SRC}"
            COMMENT "Compiling Metal shader: ${SHADER_NAME}.metal"
        )
    endforeach()

    # Путь к финальной библиотеке .metallib
    set(OUTPUT_METALLIB "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_LIB_NAME}.metallib")

    # Линкуем все .air файлы в один .metallib
    add_custom_command(
        OUTPUT ${OUTPUT_METALLIB}
        COMMAND xcrun -sdk macosx metallib ${AIR_FILES} -o "${OUTPUT_METALLIB}"
        DEPENDS ${AIR_FILES}
        COMMENT "Linking Metal library: ${OUTPUT_LIB_NAME}.metallib"
    )

    # 1. Создаем кастомный таргет для сборки шейдеров. 
    # Он гарантирует, что команда линковки metallib точно выполнится.
    set(SHADERS_TARGET "${TARGET_NAME}_${OUTPUT_LIB_NAME}_shaders")
    add_custom_target(${SHADERS_TARGET} DEPENDS ${OUTPUT_METALLIB})

    # 2. Делаем так, чтобы основное приложение зависело от сборки шейдеров
    add_dependencies(${TARGET_NAME} ${SHADERS_TARGET})

    # 3. Добавляем готовый .metallib в ресурсы Bundle приложения
    # (Чтобы CMake понял, что это ресурс, а не исходный код, пропишем GENERATED)
    set_source_files_properties(${OUTPUT_METALLIB} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET_NAME} PRIVATE ${OUTPUT_METALLIB})
    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY RESOURCE "${OUTPUT_METALLIB}")
    
endfunction()