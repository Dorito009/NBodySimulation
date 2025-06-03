cmake_minimum_required(VERSION 3.5)
project(nbody_simulation)

# Require at least C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# look for libaries
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Sources
set(SOURCES
        src/Window.cpp
        include/Window.h
        src/Shader.cpp
        include/Shader.h
        src/main.cpp
        src/Camera.cpp
        include/Camera.h
        src/SimulationHandler.cpp
        include/SimulationHandler.h
        src/Renderer.cpp
        include/Renderer.h
)

# Declare the executable
add_executable(nbody ${SOURCES})

# Link against libaries
target_link_libraries(nbody
    ${SDL2_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${GLEW_LIBRARIES}
)
