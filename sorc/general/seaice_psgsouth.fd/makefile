#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
 
include ../makeall.mk

LIBS = ${MMAB_LIBF4} ${W3EMC_LIB4} ${BACIO_LIB4}

#Should be no changes below this point---------------------------------
all : seaice_psgsouth seaice_psgsouth12

seaice_psgsouth12 : psgrib.f90 
	ln -sf $(MMAB_INC)/icegrid.south12 icegrid.inc
	$(FC) $(FOPTS)     $< 
	$(FLD) $(FLDFLAGS) psgrib.o $(LIBS) $(MMAB_LIBF4) -o $(@)
	rm -f icegrid.inc psgrib.o

seaice_psgsouth : psgrib.f90 
	ln -sf $(MMAB_INC)/icegrid.south icegrid.inc
	$(FC) $(FOPTS)     $< 
	$(FLD) $(FLDFLAGS) psgrib.o $(LIBS) $(MMAB_LIBF4) -o $(@)
	rm -f icegrid.inc psgrib.o
