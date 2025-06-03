cmake_minimum_required(VERSION 3.5)
project(nbody_simulation)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find libraries
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# ImGui paths
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)

# Include directories
include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${glm_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/src
        ${IMGUI_DIR}
        ${IMGUI_BACKENDS}
)

# ImGui library
add_library(imgui
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKENDS}/imgui_impl_sdl2.cpp
        ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
)

# Your source files
set(SOURCES
        src/main.cpp
        src/Window.cpp        include/src/Window.h
        src/Shader.cpp        include/src/Shader.h
        src/Camera.cpp        include/src/Camera.h
        depricated/SimulationHandler.cpp depricated/SimulationHandler.h
        src/Renderer.cpp      include/src/Renderer.h
        src/NewtonianSimulationHandler.cpp
        include/src/NewtonianSimulationHandler.h
        src/EventDispatcher.cpp
        include/src/EventDispatcher.h
        src/GuiHandler.cpp
        include/src/GuiHandler.h
)

# Create executable
add_executable(nbody ${SOURCES})

# Link everything
target_link_libraries(nbody
        ${SDL2_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${GLEW_LIBRARIES}
        imgui
)
