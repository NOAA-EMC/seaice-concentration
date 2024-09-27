#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
 
include ../makeall.mk

LIBS = ${MMAB_LIBF4} ${W3EMC_LIB4} ${BACIO_LIB4}

#Should be no changes below this point---------------------------------
all: psgnorth psgnorth12

psgnorth12 : 
	ln -sf $(MMAB_INC)/icegrid.north12 icegrid.inc
	$(FC) $(FOPTS)  psgrib.f90 
	$(FLD) $(FLDFLAGS) psgrib.o $(LIBS) $(MMAB_LIBF4) -o seaice_psgnorth12
	rm -f icegrid.inc psgrib.o

psgnorth : 
	ln -sf $(MMAB_INC)/icegrid.north icegrid.inc
	$(FC) $(FOPTS)   psgrib.f90 
	$(FLD) $(FLDFLAGS) psgrib.o $(LIBS) $(MMAB_LIBF4) -o seaice_psgnorth
	rm -f icegrid.inc psgrib.o
