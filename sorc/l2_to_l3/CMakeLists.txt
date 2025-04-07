list(APPEND CXX_src 
	ssmisu_tol3.C
)

set(exe_name ssmisu_tol3 )

# Need to work out how to get the referenced hdf5 functions

#add_executable(${exe_name} ${CXX_src})

#target_link_libraries(${exe_name} libombf_4.a
#	/apps/prod/hpc-stack/intel-19.1.3.304/netcdf/4.7.4/lib/libnetcdf.a
#	#/apps/prod/hpc-stack/intel-19.1.3.304/netcdf/4.7.4/lib/libnetcdf_c++4.a
#	#/apps/prod/hpc-stack/intel-19.1.3.304/hdf5/1.10.6/lib/libhdf5.a
#       	/apps/ops/prod/libs/intel/19.1.3.304/bufr/11.7.0/lib64/libbufr_d.a
#	/apps/ops/prod/libs/intel/19.1.3.304/w3nco/2.4.1/lib/libw3nco_d.a
#       	${W3EMC_LIBd}
#       	)

#install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
