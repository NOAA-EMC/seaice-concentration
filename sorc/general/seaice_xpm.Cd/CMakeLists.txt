list(APPEND CXX_src1 
  seaice_xpm.C
)
list(APPEND CXX_src2
  seaice_fxpm.C
)

#set(exe_name seaice_fxpm )
#add_executable(${exe_name} ${CXX_src2} )

set(exe_name seaice_northxpm )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=northgrid")
add_executable(${exe_name} ${CXX_src1} )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_southxpm )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=southgrid")
add_executable(${exe_name} ${CXX_src1} )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_north12xpm )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=northhigh")
add_executable(${exe_name} ${CXX_src1} )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_south12xpm )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=southhigh")
add_executable(${exe_name} ${CXX_src1} )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_global5minxpm )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=global_12th")
add_executable(${exe_name} ${CXX_src1} )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(exe_name seaice_globalxpm )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=global_ice")
add_executable(${exe_name} ${CXX_src1} )
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


