add_library(mesh-file-pkg STATIC
    include/mesh-file/FileFormat.hpp
    include/mesh-file/IMeshBuilder.hpp
    include/mesh-file/MeshLoader.hpp
    include/mesh-file/MeshSerializer.hpp
    src/MeshSerializer.cpp
)

target_include_directories(mesh-file-pkg PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mesh-file-pkg PRIVATE
    spdlog::spdlog
    ufbx_lib
)