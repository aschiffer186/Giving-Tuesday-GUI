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
file(GLOB SOURCES "src/*.cpp" test/compile_check.cpp)

add_executable(Test ${SOURCES})

