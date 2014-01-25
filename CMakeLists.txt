
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(libraw1394_tutorial)

# Set the ouptut path for the libraries and executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)   # example code
add_subdirectory(util)  # util program




