cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(libraw1394_tutorial)

# Set the ouptut path for the libraries and executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_executable(main1394 src/main1394.cpp)
target_link_libraries(main1394 raw1394)

add_executable(0_getting_started src/0_getting_started.cpp)
target_link_libraries(0_getting_started raw1394)

add_executable(1_bus_reset src/1_bus_reset.cpp)
target_link_libraries(1_bus_reset raw1394)

add_executable(2_arm_server src/2_arm_server.cpp)
target_link_libraries(2_arm_server raw1394)

add_executable(3_async_client src/3_async_client.cpp)
target_link_libraries(3_async_client raw1394)

#add_executable(4_async_broadcast src/4_async_broadcast.cpp)
#target_link_libraries(4_async_broadcast raw1394)


