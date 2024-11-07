list(APPEND CXX_src 
  seaice_iceamsr2.C
)

set(exe_name seaice_iceamsr2 )
add_executable(${exe_name} ${CXX_src})

target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a
	${W3EMC_LIB4} ${W3NCO_LIB4} ${BUFR_LIBd}
	)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
