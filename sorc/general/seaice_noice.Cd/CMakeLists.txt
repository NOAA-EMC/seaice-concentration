list(APPEND CXX_src 
 noice.C
)

set(exe_name noice )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRIDTYPE=global_12th  ")


add_executable(${exe_name} ${CXX_src}  )
target_link_libraries(${exe_name} libombf_4.a)

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
