#add_subdirectory( seaice_xpm.Cd )
#add_subdirectory( seaice_regions.fd )
add_subdirectory( seaice_blend.Cd )
add_subdirectory( seaice_monitor.Cd )
add_subdirectory( seaice_noice.Cd )
add_subdirectory( seaice_imsice.Cd )
add_subdirectory( seaice_filtanal.Cd )

#set(exe_name seaice_iceamsr2 seaice_amsrbufr )
#add_executable(${exe_name} ${fortran_src})
#
#install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
