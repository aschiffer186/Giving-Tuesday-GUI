cmake_minimum_required(VERSION 3.17.2)

#Set Project 
project(Giving-Tuesday)

#Set required C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS -std=c++2a)

#Include directory
include_directories(src/include)

#Set sources 
set(SOURCES src/basic_types.cpp test/compile_check.cpp)
#file(GLOB SOURCES src/*.cpp)

add_executable(Test ${SOURCES})

