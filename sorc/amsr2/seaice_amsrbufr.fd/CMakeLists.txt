list(APPEND c_src
  amsr2out.c
)
list(APPEND fortran_src 
  amsr2.f90
)

set(exe_name seaice_amsrbufr )

add_executable(${exe_name} ${c_src} ${fortran_src} )

#target_link_libraries(${exe_name} libombf_4.a ${BUFR_LIB4} ${W3EMC_LIB4} ${W3NCO_LIB4} )

target_link_libraries(${exe_name} libombf_4.a /apps/ops/prod/libs/intel/19.1.3.304/bufr/12.0.0/lib64/libbufr_4.a ${W3EMC_LIB4} /apps/ops/prod/libs/intel/19.1.3.304/w3nco/2.4.1/lib/libw3nco_4.a )

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
