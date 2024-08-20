list(APPEND c_src
  amsr2out.c
)
list(APPEND fortran_src 
  amsr2.f90
)

set(exe_name seaice_amsrbufr )
add_executable(${exe_name} ${c_src} ${fortran_src} )

#message("bufr lib: " "${BUFR_LIBd}")

target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a
	${W3EMC_LIB4} ${W3NCO_LIB4} ${BUFR_LIBd}
	)
install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
