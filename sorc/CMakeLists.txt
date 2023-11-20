cmake_minimum_required(VERSION 3.19)

project(try.cmake
        VERSION 1.0.0
	LANGUAGES Fortran C C++)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_DIRECTORY_LABELS ${PROJECT_NAME})

include(GNUInstallDirs)
#
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

set(CMAKE_INSTALL_PREFIX "/u/robert.grumbine/rgdev/rg.analy")
set(CMAKE_INSTALL_BINDIR "exec")

# Build and install code.
add_subdirectory(mmablib)
add_subdirectory(amsr2)

## Install utility scripts.
#add_subdirectory(ush)

