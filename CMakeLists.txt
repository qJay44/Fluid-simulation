cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "C:/MinGW/mingw32/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/mingw32/bin/g++.exe")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_DIR "src/")

project(MyProject VERSION 1.0)

find_package(SFML 2.6.0 COMPONENTS system window graphics REQUIRED)
find_package(OpenGL REQUIRED)
set(IMGUI "C:/Users/gerku/Documents/LIDL programs/imgui-1.89.3/")
set(IMGUI_SFML "C:/Users/gerku/Documents/LIDL programs/imgui-sfml/")

include_directories(${OPENGL_INCLUDE_DIRS} ${IMGUI} ${IMGUI_SFML})

add_executable(MyProject
  ${SRC_DIR}main.cpp
  ${IMGUI}imgui.cpp
  ${IMGUI}imgui_widgets.cpp
  ${IMGUI}imgui_draw.cpp
  ${IMGUI}imgui_tables.cpp
  ${IMGUI_SFML}imgui-SFML.cpp
)

target_link_libraries(MyProject sfml-system sfml-window sfml-graphics ${OPENGL_LIBRARIES})

