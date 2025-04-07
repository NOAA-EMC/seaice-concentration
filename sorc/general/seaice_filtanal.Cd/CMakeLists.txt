list(APPEND CXX_src 
 filt3.C
)

set(exe_name seaice_filtanal )


add_executable(${exe_name} ${CXX_src}  )
target_link_libraries(${exe_name} libombf_4.a)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
