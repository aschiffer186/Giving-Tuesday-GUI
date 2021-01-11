cmake_minimum_required(VERSION 3.17.2)

#Set Project 
project(Giving-Tuesday)

#Set required C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Include directory
include_directories(src/include lib lib/xlnt/include)

#Set sources 
file(GLOB SOURCES src/*.cpp lib/getopt.c)
set(COMPILE_SRC_LIST src/basic_types.cpp src/io.cpp src/matching.cpp test/compile_check.cpp)

#Set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(Command_Line ${SOURCES})
target_link_libraries(Command_Line ${CMAKE_SOURCE_DIR}/lib/xlnt/lib/xlnt.lib)

add_executable(Test ${COMPILE_SRC_LIST} lib/getopt.c)



