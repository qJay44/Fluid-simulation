cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "C:/MinGW/mingw32/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/mingw32/bin/g++.exe")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_DIR "src/")

project(MyProject VERSION 1.0)

find_package(SFML 2.6.0 COMPONENTS system window graphics REQUIRED)

add_executable(MyProject ${SRC_DIR}main.cpp ${SRC_DIR}fluid.cpp)
target_link_libraries(MyProject sfml-system sfml-window sfml-graphics)

