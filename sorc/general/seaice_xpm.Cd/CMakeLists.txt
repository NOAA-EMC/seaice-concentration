list(APPEND CXX_src1 
  seaice_xpm.C
)
list(APPEND CXX_src2
  seaice_fxpm.C
)

set(exe_name seaice_xpm )
add_executable(${exe_name} ${CXX_src1} ${CXX_src2} )

set(exe_name seaice_fxpm )
add_executable(${exe_name} ${CXX_src2} )

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
