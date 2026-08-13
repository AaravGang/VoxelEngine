cmake_minimum_required(VERSION 3.10)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Silence macOS OpenGL deprecation warnings globally
add_definitions(-DGL_SILENCE_DEPRECATION)

include_directories(include)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/glad.c")

add_executable(VoxelEngine ${SOURCES})

target_link_libraries(VoxelEngine glfw "-framework OpenGL")