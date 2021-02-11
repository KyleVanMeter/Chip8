cmake_minimum_required(VERSION 3.16)
include(ExternalProject)

project(Chip8 VERSION 0.1)

ADD_LIBRARY(otherModule chip8.cpp view.cpp IReader.cpp )
target_link_libraries(otherModule -lSDL2)

add_executable(Chip8 main.cpp)

ExternalProject_Add(spdlog
    PREFIX ${PROJECT_SOURCE_DIR}/build/spdlog
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/build
    -DSPDLOG_BUILD_SHARED=OFF
)
add_dependencies(${PROJECT_NAME} spdlog)

target_link_libraries(Chip8 otherModule)
target_include_directories(Chip8 PUBLIC "${PROJECT_SOURCE_DIR}")

add_executable(Ctest test/test.cpp)
target_link_libraries(Ctest otherModule)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
