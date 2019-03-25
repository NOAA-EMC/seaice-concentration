#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
 
include ../makeall.mk

LIBS = ${MMAB_LIB} ${W3NCO_LIB4} ${BACIO_LIB4}

#Should be no changes below this point---------------------------------
all : psgsouth psgsouth12

psgsouth12 : 
	ln -sf $(MMAB_LN)/icegrid.south12 icegrid.inc
	$(FC) $(FOPTS)     psgrib.f $(MMAB_SRC)/GRIBIT.f 
	$(FLD) $(FLDFLAGS) psgrib.o GRIBIT.o $(LIBS) -lombf_4 -o seaice_psgsouth12
	rm -f icegrid.inc psgrib.o

psgsouth : 
	ln -sf $(MMAB_LN)/icegrid.south icegrid.inc
	$(FC) $(FOPTS)     psgrib.f $(MMAB_SRC)/GRIBIT.f
	$(FLD) $(FLDFLAGS) psgrib.o GRIBIT.o $(LIBS) -lombf_4 -o seaice_psgsouth
	rm -f icegrid.inc psgrib.o
