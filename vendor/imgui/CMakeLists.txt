cmake_minimum_required(VERSION 3.0.0)
project(IMGUI VERSION 0.1.0)

include_directories(vendors/imgui)

add_library(
  IMGUI SHARED
  
  imconfig.h
  imgui.cpp
  imgui.h
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_internal.h
  imgui_widgets.cpp
  imgui_tables.cpp
  imstb_rectpack.h
  imstb_textedit.h
  imstb_truetype.h

  backends/imgui_impl_glfw.cpp
  backends/imgui_impl_opengl3.cpp
  misc/cpp/imgui_stdlib.cpp
)