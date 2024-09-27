list(APPEND cxx_src
	gacfilt.C
)

set(exe_name seaice_avhrrfilter )
add_executable(${exe_name} ${cxx_src} )

target_link_libraries(${exe_name} libombf_4.a
	)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
