cmake_minimum_required(VERSION 3.10)

project(Chip8 VERSION 0.1)
ADD_LIBRARY(otherModule chip8.cpp view.cpp IReader.cpp )
target_link_libraries(otherModule -lSDL2 -L/usr/lib)

add_executable(Chip8 main.cpp)

target_link_libraries(Chip8 otherModule)
target_include_directories(Chip8 PUBLIC "${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
