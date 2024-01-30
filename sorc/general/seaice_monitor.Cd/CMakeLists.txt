list(APPEND CXX_src 
 seaice_monitor.C
)

set(exe_name seaice_monitor )
add_executable(${exe_name} ${CXX_src}  )

set(CMAKE_CXX_FLAGS "${CMAKE_Fortran_FLAGS} -DGRIDTYPE=global_12th -DDATTYPE=\"unsigned char\" ")
target_link_libraries(${exe_name} libombf_4.a)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
