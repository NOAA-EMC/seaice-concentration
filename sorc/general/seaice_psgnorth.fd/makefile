#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
 
include ../makeall.mk

LIBS = ${MMAB_LIB} ${W3NCO_LIB4} ${BACIO_LIB4}

#Should be no changes below this point---------------------------------
all: psgnorth psgnorth12

psgnorth12 : 
	ln -sf $(MMAB_LN)/icegrid.north12 icegrid.inc
	$(FC) $(FOPTS)  psgrib.f $(MMAB_SRC)/GRIBIT.f
	$(FLD) $(FLDFLAGS) psgrib.o GRIBIT.o $(LIBS) -lombf_4 -o seaice_psgnorth12
	rm -f icegrid.inc psgrib.o

psgnorth : 
	ln -sf $(MMAB_LN)/icegrid.north icegrid.inc
	$(FC) $(FOPTS)   psgrib.f $(MMAB_SRC)/GRIBIT.f
	$(FLD) $(FLDFLAGS) psgrib.o GRIBIT.o $(LIBS) -lombf_4 -o seaice_psgnorth
	rm -f icegrid.inc psgrib.o
