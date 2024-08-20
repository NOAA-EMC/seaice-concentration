list(APPEND CXX_src 
 imsice.C
)

set(exe_name imsice )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=global_12th  ")


add_executable(${exe_name} ${CXX_src}  )
#target_link_libraries(${exe_name} libombf_4.a)
target_link_libraries(${exe_name} ${CMAKE_INSTALL_PREFIX}/sorc/mmablib/lib/libombf_4.a)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
