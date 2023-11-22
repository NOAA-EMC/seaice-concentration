add_subdirectory( subdir.fd )
add_subdirectory( c_openmp)

list(APPEND fortran_src
a.f90
)

set(exe_name alpha)
add_executable(${exe_name} ${fortran_src})

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
