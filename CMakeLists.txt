
cmake_minimum_required(VERSION 3.1.0)
project(szakdolgozat)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

include_directories(glfw/include glm glad-build/include tinyobjloader stb nanogui/include nanogui/ext/Eigen)
link_directories(glfw-build/src nanogui-build)

set(GLAD_sources
    glad-build/include/glad/glad.h
    glad-build/src/glad.c
)

set(tinyobjloader_sources
    tinyobjloader/tiny_obj_loader.h
    tinyobjloader/tiny_obj_loader.cc
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
