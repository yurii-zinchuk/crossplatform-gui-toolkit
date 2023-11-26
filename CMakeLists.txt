cmake_minimum_required(VERSION 3.10)

project(gui_toolkit)

set(CMAKE_CXX_STANDARD 20)

add_executable(gui_toolkit main.cpp elements/MyButton.cpp  elements/MyText.cpp user_program.cpp elements/MyTextInput.cpp)

# Only find and link X11 on Unix/Linux systems
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        include_directories(${X11_INCLUDE_DIR})
        target_link_libraries(gui_toolkit PRIVATE ${X11_LIBRARIES})
    endif()
endif()
