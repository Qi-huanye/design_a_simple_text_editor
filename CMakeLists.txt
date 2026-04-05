cmake_minimum_required(VERSION 3.10)
project(text_editor)

find_package(FLTK 1.4 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(text_editor main.cpp textEditor.cpp editorWindow.cpp)
target_link_libraries(text_editor PRIVATE fltk::fltk)