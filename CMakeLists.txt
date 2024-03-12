cmake_minimum_required(VERSION 3.6)
project(VoiceRig VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
include_directories(${CMAKE_SOURCE_DIR}/vendor/ImGuizmo)
enable_testing()

# GLFW
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/vendor/glad)
include_directories(${CMAKE_SOURCE_DIR}/vendor/imgui)

# Pipeware using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(Pipewire REQUIRED libpipewire-0.3)
include_directories(${Pipewire_INCLUDE_DIRS})
message(STATUS "Pipewire include dirs: ${Pipewire_INCLUDE_DIRS}")

add_subdirectory(vendor/ImGuizmo)

add_executable(ImGuiWindowGL 
  vendor/imgui/imgui_demo.cpp
  vendor/imgui/imgui_draw.cpp
  vendor/imgui/imgui_widgets.cpp
  vendor/imgui/imgui_tables.cpp
  vendor/imgui/imgui.cpp
  vendor/imgui/backends/imgui_impl_glfw.cpp
  vendor/imgui/backends/imgui_impl_opengl3.cpp
  vendor/imgui/misc/cpp/imgui_stdlib.cpp

  glad.c 
  Theme.cpp
  main.cpp
)

target_link_libraries(ImGuiWindowGL glfw IMGUIZMO ${Pipewire_LIBRARIES} GL dl pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
