list(APPEND c_src
  ssmisu_binary.c
)
list(APPEND fortran_src 
  ssmisu_decode.f90
)

set(exe_name seaice_ssmisubufr )
add_executable(${exe_name} ${c_src} ${fortran_src} )

target_link_libraries(${exe_name} libombf_4.a
	/apps/ops/prod/libs/intel/19.1.3.304/bufr/11.7.0/lib64/libbufr_d.a
	${W3EMC_LIB4}
	/apps/ops/prod/libs/intel/19.1.3.304/w3nco/2.4.1/lib/libw3nco_d.a
	)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
