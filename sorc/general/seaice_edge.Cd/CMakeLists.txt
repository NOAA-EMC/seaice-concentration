list(APPEND CXX_src 
 seaice_edge.C
)

set(exe_name seaice_edge )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMODEL=global_12th -DANALYSIS=global_12th")


add_executable(${exe_name} ${CXX_src}  )
#target_link_libraries(${exe_name} libombf_4.a)
target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombf_4.a)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
