cmake_minimum_required(VERSION 3.10)
project(grav_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Tell CMake where to find GLFW installed by Homebrew
find_package(glfw3 REQUIRED PATHS /opt/homebrew/opt/glfw /usr/local/opt/glfw)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    glfw
    OpenGL::GL
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)

