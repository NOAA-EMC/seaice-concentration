list(APPEND CXX_src 
  regions.C
)


set(exe_name seaice_regions.north  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=northgrid") 
add_executable(${exe_name} ${CXX_src}  )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_regions.south  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=southgrid") 
add_executable(${exe_name} ${CXX_src}  )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_regions.northhigh  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=northhigh") 
add_executable(${exe_name} ${CXX_src}  )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_regions.southhigh  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=southhigh") 
add_executable(${exe_name} ${CXX_src}  )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})




