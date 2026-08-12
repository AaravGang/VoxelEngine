cmake_minimum_required(VERSION 3.10)
project(VoxelEngine)

# Enforce C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header directories
include_directories(include)

# Find macOS specific packages
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Gather all source files
file(GLOB SOURCES "src/*.cpp" "src/glad.c")

# Create the executable
add_executable(VoxelEngine ${SOURCES})

# Link libraries (macOS requires the framework link)
target_link_libraries(VoxelEngine glfw "-framework OpenGL")