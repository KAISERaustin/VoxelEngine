cmake_minimum_required(VERSION 3.10)

# Executable target
add_executable(VoxelEngine)

# C++ standard
target_compile_features(VoxelEngine PRIVATE cxx_std_20)

# Absolute paths (prevents relative-path confusion in VS CMake folder mode)
set(VOXEL_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(VOXEL_INC_DIR  "${VOXEL_ROOT_DIR}/include")
set(VOXEL_SRC_DIR  "${VOXEL_ROOT_DIR}/src")

# Include path for headers like: #include "platform/Window.h"
target_include_directories(VoxelEngine
    PRIVATE
        "${VOXEL_INC_DIR}"
)

# All source files for the executable
target_sources(VoxelEngine
    PRIVATE
        "${VOXEL_SRC_DIR}/main.cpp"

        "${VOXEL_SRC_DIR}/platform/Window.cpp"
        "${VOXEL_SRC_DIR}/platform/Input.cpp"

        "${VOXEL_SRC_DIR}/render/Camera.cpp"
        "${VOXEL_SRC_DIR}/render/CameraController.cpp"
        "${VOXEL_SRC_DIR}/render/GLShader.cpp"
        "${VOXEL_SRC_DIR}/render/TextureArray.cpp"
        "${VOXEL_SRC_DIR}/render/Renderer.cpp"

        "${VOXEL_SRC_DIR}/world/Noise.cpp"
        "${VOXEL_SRC_DIR}/world/World.cpp"

        "${VOXEL_SRC_DIR}/mesh/VoxelMesher.cpp"
)

# vcpkg packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Link libraries
target_link_libraries(VoxelEngine
    PRIVATE
        glfw
        glad::glad
        glm::glm
        opengl32
)

# Optional: warnings (MSVC)
if (MSVC)
    target_compile_options(VoxelEngine PRIVATE /W4)
endif()
