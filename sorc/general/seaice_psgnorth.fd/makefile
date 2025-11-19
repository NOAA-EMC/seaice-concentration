#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
 
include ../makeall.mk

LIBS = ${MMAB_LIBF4} ${W3EMC_LIB4} ${BACIO_LIB4}

#Should be no changes below this point---------------------------------
all: seaice_psgnorth seaice_psgnorth12

seaice_psgnorth12 : psgrib.f90 
	ln -sf $(MMAB_INC)/icegrid.north12 icegrid.inc
	$(FC) $(FOPTS)  $<
	$(FLD) $(FLDFLAGS) psgrib.o $(LIBS) $(MMAB_LIBF4) -o $(@)
	rm -f icegrid.inc psgrib.o

seaice_psgnorth : psgrib.f90 
	ln -sf $(MMAB_INC)/icegrid.north icegrid.inc
	$(FC) $(FOPTS)   $<
	$(FLD) $(FLDFLAGS) psgrib.o $(LIBS) $(MMAB_LIBF4) -o $(@)
	rm -f icegrid.inc psgrib.o
