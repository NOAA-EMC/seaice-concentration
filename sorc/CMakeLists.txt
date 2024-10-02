cmake_minimum_required(VERSION 3.19)

project(seaice_analysis
        VERSION 1.0.0
	LANGUAGES Fortran C CXX)

if (CMAKE_Fortran_COMPILER_ID MATCHES "GNU" AND CMAKE_Fortran_COMPILER_VERSION VERSION_LESS 9.0.0)
  message(FATAL_ERROR "GNU Compiler >= 9 is required")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules/Modules)

set(CMAKE_DIRECTORY_LABELS ${PROJECT_NAME})

message("")

message("..${CMAKE_Fortran_COMPILER_ID}")
set(CMAKE_Platform $ENV{CMAKE_Platform})
if(CMAKE_Platform)
  message("")
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_${CMAKE_Platform}.cmake)
    message("Setting configuration for ${CMAKE_Platform}")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_${CMAKE_Platform}.cmake)
  else()
    message("Platform '${CMAKE_Platform}' configuration file does not exist")
  endif()
endif()

###############################################################################
### Set CMAKE_BUILD_TYPE for DEBUG mode
###############################################################################
if(DEBUG)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set type of build to Debug." FORCE)
else()
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Set type of build to Release." FORCE)
endif()

include(cmake/${CMAKE_Fortran_COMPILER_ID}.cmake)

###############################################################################
### Find Dependencies
###############################################################################
find_package(MPI REQUIRED)
if(OPENMP)
  find_package(OpenMP REQUIRED)
endif()

find_package(NetCDF 4.7.4 REQUIRED C Fortran)


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
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -DLINUX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -Wall -std=c++11 -DCPLUS ")

#set(CMAKE_INSTALL_PREFIX "/u/robert.grumbine/rgdev/rg.analy")
set(CMAKE_INSTALL_BINDIR "exec")

include_directories(mmablib/include)

# Build and install code -- mmablib
add_subdirectory(mmablib)
# -- need to work out: link_libraries(libombf_4.a)
#target_link_libraries(/u/robert.grumbine/rgdev/rg.analy/sorc/mmablib/libombf_4.a)
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

###############################################################################
### Install
###############################################################################
#install(TARGETS seaice_analysis)

#install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mod DESTINATION ${CMAKE_INSTALL_PREFIX})

###############################################################################
### Done
###############################################################################
