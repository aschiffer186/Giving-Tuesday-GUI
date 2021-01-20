cmake_minimum_required(VERSION 3.17.2)

#Set Project 
project(Giving-Tuesday LANGUAGE C CXX)

#Set required C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_)

#Set sources 
file(GLOB SOURCES src/*.cpp src/Analysis/*.cpp src/Command_Line_UI/*.cpp src/File_IO/*.cpp lib/getopt.c)
include_directories(src/ lib/include)
#Set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(Command_Line ${SOURCES})



