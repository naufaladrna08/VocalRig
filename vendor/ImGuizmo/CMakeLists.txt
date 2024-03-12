cmake_minimum_required(VERSION 3.0.0)
project(IMGUIZMO VERSION 0.1.0)

include_directories(../vendors/imgui)

add_library(
  IMGUIZMO SHARED
  
  GraphEditor.cpp 
  GraphEditor.h
  ImCurveEdit.cpp
  ImCurveEdit.h
  ImGradient.cpp
  ImGradient.h
  ImSequencer.cpp
  ImSequencer.h
  ImGuizmo.cpp
  ImGuizmo.h
  ImSequencer.cpp
  ImSequencer.h
  ImZoomSlider.h
)