cmake_minimum_required(VERSION 3.10)
project(grav_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL everywhere
find_package(OpenGL REQUIRED)

# -------------------------
# Platform-specific settings
# -------------------------
if (WIN32)
    # GLFW from MSYS2/MinGW
    include_directories("C:/msys64/mingw64/include")
    link_directories("C:/msys64/mingw64/lib")
    set(GLFW_LIB glfw3)

elseif(APPLE)
    # GLFW from Homebrew/macOS
    find_package(glfw3 REQUIRED PATHS /opt/homebrew/opt/glfw /usr/local/opt/glfw)
    set(GLFW_LIB glfw)

    # Silence deprecation warnings on macOS
    add_compile_definitions(GL_SILENCE_DEPRECATION)

else()
    # Linux (or other Unix)
    find_package(glfw3 REQUIRED)
    set(GLFW_LIB glfw)
endif()

# -------------------------
# Build executable
# -------------------------
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${GLFW_LIB}
    OpenGL::GL
)

# Extra Windows libs required by GLFW
if (WIN32)
    target_link_libraries(${PROJECT_NAME} gdi32 user32 shell32)
endif()
