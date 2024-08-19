cmake_minimum_required(VERSION 3.19)

project(try.cmake
        VERSION 1.0.0
	LANGUAGES Fortran C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_DIRECTORY_LABELS ${PROJECT_NAME})

include(GNUInstallDirs)

## Build type
#if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release|RelWithDebInfo|MinSizeRel)$")
#  message(STATUS "Setting build type to 'Release' as none was specified.")
#  set(CMAKE_BUILD_TYPE
#      "Release"
#      CACHE STRING "Choose the type of build." FORCE)
#  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
#                                               "MinSizeRel" "RelWithDebInfo")
#endif()
#
#if(NOT CMAKE_Fortran_COMPILER_ID MATCHES "^(GNU|Intel)$")
#  message(WARNING "${CMAKE_Fortran_COMPILER_ID} is not supported.")
#endif()
#

# Set compiler flags.
if(CMAKE_Fortran_COMPILER_ID MATCHES "^(Intel)$")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -g -traceback")
  set(CMAKE_Fortran_FLAGS_RELEASE "-O3")
  set(CMAKE_Fortran_FLAGS_DEBUG "-O0 -check -check noarg_temp_created -check nopointer -fp-stack-check -fstack-protector-all -fpe0 -debug -ftrapuv")
elseif(CMAKE_Fortran_COMPILER_ID MATCHES "^(GNU)$")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -g -fbacktrace")
  set(CMAKE_Fortran_FLAGS_RELEASE "-O3")
  set(CMAKE_Fortran_FLAGS_DEBUG "-O0 -ggdb -fno-unsafe-math-optimizations -frounding-math -fsignaling-nans -ffpe-trap=invalid,zero,overflow -fbounds-check")
endif()
#-----------------------------------------

#For nceplibs:

# for RG
set(HOME "/home/Robert.Grumbine")
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -DLINUX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -Wall -std=c++11 -DCPLUS ")

#wcoss2: set(CMAKE_INSTALL_PREFIX "/u/robert.grumbine/rgdev/rg.analy")
#hera: 
set(CMAKE_INSTALL_PREFIX "/home/Robert.Grumbine/rgdev/rg.analy")

set(CMAKE_INSTALL_BINDIR "exec")
set(CMAKE_INSTALL_LIBDIR "lib")

include_directories(mmablib/include)
include_directories($NETCDF_INCLUDE)

# Build and install code -- mmablib
add_subdirectory(mmablib)
# -- need to work out: link_libraries(libombf_4.a)
#link_libraries(${W3NCO_LIBd})

# Executables
add_subdirectory(general)
add_subdirectory(amsr2)
add_subdirectory(avhrr)
add_subdirectory(ssmis)
add_subdirectory(l2_to_l3)
add_subdirectory(l1b_to_l2)

## Install utility scripts.
#add_subdirectory(ush)
