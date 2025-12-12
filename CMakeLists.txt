cmake_minimum_required(VERSION 3.20)
project(VoxelEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Libraries from vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Main executable (adjust path if your .cpp moves)
add_executable(VoxelEngine
    VoxelEngine/VoxelEngine.cpp
)

target_link_libraries(VoxelEngine PRIVATE
    glfw
    glm::glm
    glad::glad
    OpenGL::GL
)

if (MSVC)
    target_compile_options(VoxelEngine PRIVATE /W4 /permissive-)
endif()
