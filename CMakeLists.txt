cmake_minimum_required(VERSION 3.17.2)

#Set Project 
project(Giving-Tuesday)

#Set required C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_)

#Set sources 
file(GLOB SOURCES src/*.cpp lib/getopt.c)
include_directories(src/include/ lib/include)
#Set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(Command_Line ${SOURCES})



