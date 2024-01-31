add_subdirectory( seaice_blend.Cd )
add_subdirectory( seaice_monitor.Cd )
add_subdirectory( seaice_noice.Cd )
add_subdirectory( seaice_imsice.Cd )
add_subdirectory( seaice_filtanal.Cd )
add_subdirectory( seaice_edge.Cd )
add_subdirectory( seaice_icegrid.Cd )

#add_subdirectory( seaice_xpm.Cd )
#add_subdirectory( seaice_regions.fd )

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
