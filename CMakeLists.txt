
cmake_minimum_required(VERSION 3.1.0)
project(szakdolgozat)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(DEPS_DIR dependencies)

include_directories(
    ${DEPS_DIR}/glfw/include
    ${DEPS_DIR}/glm
    ${DEPS_DIR}/glad-build/include
    ${DEPS_DIR}/tinyobjloader
    ${DEPS_DIR}/stb
    ${DEPS_DIR}/nanogui/include
    ${DEPS_DIR}/nanogui/ext/Eigen
)

link_directories(
    ${DEPS_DIR}/glfw-build/src
    ${DEPS_DIR}/nanogui-build
)

set(GLAD_sources
    ${DEPS_DIR}/glad-build/include/glad/glad.h
    ${DEPS_DIR}/glad-build/src/glad.c
)

set(tinyobjloader_sources
    ${DEPS_DIR}/tinyobjloader/tiny_obj_loader.h
    ${DEPS_DIR}/tinyobjloader/tiny_obj_loader.cc
)

set(SOURCES
    src/AABB.h
    src/AABB.cpp
    src/Camera.h
    src/Camera.cpp
    src/FrameBuffer.h
    src/FrameBuffer.cpp
    src/FullscreenQuad.h
    src/FullscreenQuad.cpp
    src/main.cpp
    src/lights.h
    src/lights.cpp
    src/log.h
    src/GUI.h
    src/GUI.cpp
    src/materials.h
    src/materials.cpp
    src/Mesh.h
    src/Mesh.cpp
    src/Program.h
    src/Program.cpp
    src/Renderer.h
    src/Renderer.cpp
    src/Settings.h
    src/Settings.cpp
    src/Shader.h
    src/Shader.cpp
    src/Skybox.h
    src/Skybox.cpp
    src/Texture.h
    src/Texture.cpp
    src/TextureArray.h
    src/TextureArray.cpp
    src/Texture2D.h
    src/Texture2D.cpp
    src/TextureCubeMap.h
    src/TextureCubeMap.cpp
    src/Vertex.h
    src/VertexArray.h
    src/VertexArray.cpp
    src/VertexBuffer.h
    src/VertexBuffer.cpp
    src/VertexFormatDescriptor.h
    src/VertexFormatDescriptor.cpp
    src/Window.h
    src/Window.cpp
    ${GLAD_sources}
    ${tinyobjloader_sources}
)

# Visual Studio specific
source_group(glad FILES ${GLAD_sources})
source_group(tinyobjloader FILES ${tinyobjloader_sources})

# ---
add_executable(pbr_renderer ${SOURCES})

target_link_libraries(pbr_renderer glfw3 nanogui)
