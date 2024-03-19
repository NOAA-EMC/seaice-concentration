add_subdirectory( seaice_ssmisubufr.fd )
add_subdirectory( seaice_seaissmisu.cd )

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#target_link_libraries(${exe_name} libombf_4.a
#       	/apps/ops/prod/libs/intel/19.1.3.304/bufr/12.0.0/lib64/libbufr_4.a
#       	${W3EMC_LIB4}
#       	/apps/ops/prod/libs/intel/19.1.3.304/w3nco/2.4.1/lib/libw3nco_4.a
#       	)
