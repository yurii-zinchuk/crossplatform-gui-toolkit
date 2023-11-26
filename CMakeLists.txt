cmake_minimum_required(VERSION 3.10)

project(gui_toolkit)

set(CMAKE_CXX_STANDARD 11)

add_executable(gui_toolkit main.cpp elements/MyButton.cpp  elements/MyText.cpp user_program.cpp elements/MyTextInput.cpp)

find_package(X11 REQUIRED)
target_link_libraries(gui_toolkit PRIVATE X11)
