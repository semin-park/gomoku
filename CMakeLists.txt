cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(Gomoku)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pedantic")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
endif()

include_directories(/usr/local/include)

set(CMAKE_PREFIX_PATH /usr/local/libtorch)
find_package(Torch REQUIRED)

add_library(gomoku gomoku.cpp)
add_executable(main main.cpp gomoku.cpp)

target_link_libraries(gomoku "${TORCH_LIBRARIES}")
set_property(TARGET gomoku PROPERTY CXX_STANDARD 17)
set_property(TARGET main PROPERTY CXX_STANDARD 17)
