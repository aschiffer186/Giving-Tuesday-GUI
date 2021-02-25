cmake_minimum_required(VERSION 3.17.2)

#Set Project 
project(Fundraising-Analysis)

#Set required C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Set sources 
file(GLOB SOURCES src/*.cpp src/Analysis/*.cpp src/Command_Line_UI/*.cpp src/File_IO/*.cpp lib/getopt.c)
#Set binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Add executable to run on command line
add_executable(Command_Line ${SOURCES})
target_include_directories(Command_Line PRIVATE src/ lib/include/)

#Add executable for UI
add_executable(Fundraising_Analysis ${SOURCES})
#Add macro to use UI
target_compile_definitions(Fundraising_Analysis PUBLIC FUNDRAISING_USE_UI)
target_include_directories(Fundraising_Analysis PRIVATE src/ lib/include/)

set(RELEASE_OPTIONS "-O3")
target_compile_options(Command_Line PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")
target_compile_options(Fundraising_Analysis PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

#Install target 
set (CMAKE_INSTALL_PREFIX "../Giving Tuesday")
install(TARGETS Command_Line)


