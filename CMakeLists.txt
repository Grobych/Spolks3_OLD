cmake_minimum_required(VERSION 2.8.4)
project(l4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        functions.cpp
        functions.h
        main.cpp
        matrix.cpp
        matrix.h
        mympi.cpp
        mympi.h)

add_executable(l4 ${SOURCE_FILES})


find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
target_link_libraries(l4 ${MPI_LIBRARIES})