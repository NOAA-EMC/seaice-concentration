#Robert Grumbine 27 May 2016
SHELL=/bin/sh
        
include ../makeall.mk

#Demand that these be set in environment:
#MMAB_BASE=/u/Robert.Grumbine/rgdev/mmablib
VER=""
#
MMAB_INC=-I $(MMAB_BASE)/$(VER)/include/
MMAB_LIB=-L $(MMAB_BASE)/$(VER)/
MMAB_SRC=$(MMAB_BASE)/$(VER)/sorc/

#------------------- should need no changes below here ------------------------
#Compilers and their options
FC=ftn
FOPTS=-c -O2 $(MMAB_INC)
#FOPTS=-c -O2 $(MMAB_INC) $(NETCDF_INCLUDE)

FLD=$(FC)
FLDFLAGS=

#CC=gcc
COPTS=-c -ansi -O2 -DLINUX $(MMAB_INC) $(NETCDF_INCLUDE)

#CPP=g++
CPP=$(CC)
#CPPOPTS= -c -ansi -Wall -O2 -DLINUX -DCPLUS -I$(MMAB_INC) $(NETCDF_INCLUDE)
CPPOPTS= -c -ansi -Wall -O2 -DLINUX -DCPLUS $(MMAB_INC) $(NETCDF_INCLUDE)
#CPPLD=g++
CPPLD=$(CC)
CPPLDFLAGS=

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
#work -- from makeall.mk or makeall.sh as a rule
#
#NCEPLIB=-L /Users/rmg3/usrlocal/nceplibs/lib
#LIBS=-l ombf_4 -l bufr_4_DA -l w3nco_4 
LIBS=$(MMAB_BASE)/libombf_4.a
NCEPLIB=$(BUFR_LIB4) $(W3EMC_LIB4) $(W3NCO_LIB4)

#Should be no changes below this point---------------------------------

all : skip

skip : skip.o 
	$(CPP) skip.o $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

# ---------------------------------------------------------------------
clean :
	rm *.o
# ---------------------------------------------------------------------
