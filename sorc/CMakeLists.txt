cmake_minimum_required(VERSION 3.19)

project(rg.analy
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
#wcoss2: 
set(HOME "/u/robert.grumbine")
#Hera:
#set(HOME "/home/Robert.Grumbine")

#wcoss2: set(CMAKE_INSTALL_PREFIX "${HOME}/rgdev/rg.analy")
#hera: 
set(CMAKE_INSTALL_PREFIX "${HOME}/rgdev/rg.analy")
message("top level " CMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

set(CMAKE_INSTALL_BINDIR "exec")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
message(CMAKE_INSTALL_BINDIR="${CMAKE_INSTALL_BINDIR}")
message(CMAKE_INSTALL_LIBDIR="${CMAKE_INSTALL_LIBDIR}")

include_directories(mmablib/include)

set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -DLINUX -stand f08 ")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -Wall -std=c++11 -DCPLUS ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -Wall -std=c++17 -DCPLUS ")

# Build and install code -- mmablib
add_subdirectory(mmablib)
# -- need to work out: link_libraries(libombf_4.a)
#Load the relevant modules before calling the cmake:
set(W3EMC_LIB4 $ENV{W3EMC_LIB4})
set(W3NCO_LIB4 $ENV{W3NCO_LIB4})
set(BUFR_LIBd  $ENV{BUFR_LIBd})
include_directories(NETCDF_INCLUDE $ENV{NETCDF_INCLUDE})
message("Library paths w3emc " "${W3EMC_LIB4}")
message("Library paths w3nco " "${W3NCO_LIB4}")
message("Library paths bufr " "${BUFR_LIBd}")

# Executables
add_subdirectory(general)
add_subdirectory(amsr2)
add_subdirectory(avhrr)
add_subdirectory(ssmis)
add_subdirectory(l2_to_l3)
add_subdirectory(l1b_to_l2)

## Install utility scripts.
#add_subdirectory(ush)
