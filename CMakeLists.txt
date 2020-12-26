cmake_minimum_required(VERSION 3.17.2)

#Set Project 
project(Giving-Tuesday)

#Set required C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS -std=c++2a)

#Include directory
include_directories(src/include lib)

#Set sources 
file(GLOB SOURCES src/*.cpp test/compile_check.cpp)
set(COMPILE_SRC_LIST src/basic_types.cpp src/io.cpp src/matching.cpp test/compile_check.cpp)

#Set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(Test ${COMPILE_SRC_LIST} lib/getopt.c)
add_executable(Test_IO test/test_io.cpp src/io.cpp src/basic_types.cpp)

