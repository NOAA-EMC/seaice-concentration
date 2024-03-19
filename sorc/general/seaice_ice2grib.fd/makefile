#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997

include ../makeall.mk
 
LIBS = ${MMAB_LIBF4} ${W3EMC_LIB4} ${BACIO_LIB4}

#Should be no changes below this point---------------------------------
all: seaice_ice2grib seaice_ice2grib5min

seaice_ice2grib5min : mkllglob5.o 
	$(FLD) $(FLDFLAGS) mkllglob5.o $(LIBS) $(MMAB_LIBF4) -o seaice_ice2grib5min
	rm -f mkllglob5.o icegrid.inc

seaice_ice2grib : mkllglob.o 
	$(FLD) $(FLDFLAGS) mkllglob.o $(LIBS) $(MMAB_LIBF4) -o seaice_ice2grib
	rm -f mkllglob.o icegrid.inc

mkllglob5.o : mkllglob.f90
	ln -sf $(MMAB_INC)/icegrid.global5min icegrid.inc
	$(FC) $(FOPTS) mkllglob.f90 -o $(@)
 
mkllglob.o : mkllglob.f90
	ln -sf $(MMAB_INC)/icegrid.global icegrid.inc
	$(FC) $(FOPTS) mkllglob.f90  -o $(@)

