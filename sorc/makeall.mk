#Definitions to be shared by all makefiles
#Robert Grumbine 8 March 2016
SHELL=/bin/sh

#Demand that MMAB_BASE, VER be set in environment:
MMAB_LIB=$(MMAB_BASE)/$(VER)/
MMAB_INC=$(MMAB_BASE)/$(VER)/include/
MMAB_SRC=$(MMAB_BASE)/$(VER)/sorc/

#------------------- should need no changes below here ------------------------
#Compilers and their options
FC=ifort
FOPTS=-c -O2 -I $(MMAB_INC)

##Home desk:
#FC=gfortran
#FOPTS=-c -std=f95 -O2 -I $(MMAB_INC) -I $(NETCDF_INCLUDE)

FLD=$(FC)
FLDFLAGS=$(MMAB_LIBF4)

CC=gcc
COPTS=-c -ansi -O2 -DLINUX -I $(MMAB_INC) -I $(NETCDF_INCLUDE)

CPP=g++
CPPOPTS= -c -ansi -Wall -O2 -DLINUX -DCPLUS -I $(MMAB_INC) -I $(NETCDF_INCLUDE)
CPPLD=g++
CPPLDFLAGS=$(MMAB_LIBF4)

#Building elements
%.o: %.C
	$(CPP) $(CPPOPTS) $< -o $(@)
%.o: %.c
	$(CC) $(COPTS) $< -o $(@)

%.o: %.f
	$(FC) $(FOPTS) $< -o $(@)
%.o: %.F
	$(FC) $(FOPTS) $< -o $(@)
%.o: %.F90
	$(FC) $(FOPTS) $< -o $(@)
%.o: %.f90
	$(FC) $(FOPTS) $< -o $(@)
