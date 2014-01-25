
set(PROGRAMS
  0_getting_started
  1_bus_reset
  2_arm_server
  3_async_client
  4_async_broadcast
  5_iso_recv)

foreach(program ${PROGRAMS})
  add_executable(${program} ${program}.cpp)
  target_link_libraries(${program} raw1394)
endforeach(program)
