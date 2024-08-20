list(APPEND cxx_src
  ssmis.main.C
  # The following are included by ssmis.main.C:
  #algorithm.C
  #filt.C
  #getfld.C
  #hires.C
  #icetools.C
  #pole_fill.C
  #process_bufr.C
  #team2.C
)

set(exe_name seaice_seaissmisu )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTEAM1")
add_executable(${exe_name} ${cxx_src} )

target_link_libraries(${exe_name} ${CMAKE_INSTALL_LIBDIR}/libombc_4.a
        )

install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
