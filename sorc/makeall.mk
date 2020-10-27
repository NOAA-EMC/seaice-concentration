#Definitions to be shared by all makefiles
#Robert Grumbine 8 March 2016
SHELL=/bin/sh

#Demand that these be set in environment:
#BASE=$(BASE)
#VER=$(MMAB_VER)
MMAB_INC=-I $(BASE)/$(VER)/include/
MMAB_LN=$(BASE)/$(VER)/include/
MMAB_LIB=-L $(BASE)/$(VER)/
MMAB_SRC=$(BASE)/$(VER)/sorc/

#------------------- should need no changes below here ------------------------
#Compilers and their options
FC=ifort
FOPTS=-c -O2 $(MMAB_INC)
#FOPTS=-c -O2 $(MMAB_INC) $(NETCDF_INCLUDE)

##Home desk:
#FC=gfortran
###FOPTS=-c -std=f95 -O2 -I$(MMAB_INC) $(NETCDF_INCLUDE)

FLD=$(FC)
FLDFLAGS=

CPP=g++
#CPPOPTS= -c -ansi -Wall -O2 -DLINUX -DCPLUS -I$(MMAB_INC) $(NETCDF_INCLUDE)
CPPOPTS= -c -ansi -Wall -O2 -DLINUX -DCPLUS $(MMAB_INC) $(NETCDF_INCLUDE)
CPPLD=g++
CPPLDFLAGS=-lombf_4 -lombc_4

CC=gcc
COPTS=-c -ansi -O2 -DLINUX $(MMAB_INC) $(NETCDF_INCLUDE)

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
