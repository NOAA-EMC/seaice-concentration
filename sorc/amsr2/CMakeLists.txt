add_subdirectory( seaice_amsrbufr.fd )
add_subdirectory( seaice_iceamsr2.Cd )


install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
