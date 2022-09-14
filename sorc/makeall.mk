#Definitions to be shared by all makefiles
#Robert Grumbine 8 March 2016
SHELL=/bin/sh

#Demand that these be set in environment:
#MMAB_BASE=$(MMAB_BASE)
#VER=$(MMAB_VER)
#MMAB_INC=-I $(MMAB_BASE)/$(VER)/include/
#MMAB_LIB=-L $(MMAB_BASE)/$(VER)/
#MMAB_SRC=$(MMAB_BASE)/$(VER)/sorc/

#------------------- should need no changes below here ------------------------
#Compilers and their options
FC=ftn
FOPTS=-c -O2 -I $(MMAB_INC) -e03
#FOPTS=-c -O2 $(MMAB_INC) $(NETCDF_INCLUDE)

##Home desk:
#FC=gfortran
#FOPTS=-c -std=f95 -O2 $(MMAB_INC) $(NETCDF_INCLUDE)
#FOPTS=-c -O2 $(MMAB_INC) $(NETCDF_INCLUDE)

FLD=$(FC)
FLDFLAGS=$(MMAB_LIBF4)

NETCDF_LDFLAGS_C=-L$(NETCDF_LIBRARIES)

CC=cc
#gnu:
COPTS=-c -ansi -O2 -DLINUX -I $(MMAB_INC) $(NETCDF_INCLUDE)
#intel:
COPTS=-c -O2 -DLINUX -I $(MMAB_INC) $(NETCDF_INCLUDE) -std=c11

CPP=CC
CPPLD=CC
#gnu:
CPPOPTS= -c -ansi -Wall -O2 -DLINUX -DCPLUS -I $(MMAB_INC) $(NETCDF_INCLUDE)
#intel:
CPPOPTS= -c -Wall -O2 -DLINUX -DCPLUS -I $(MMAB_INC) $(NETCDF_INCLUDE) -std=c++11
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
