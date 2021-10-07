#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
  
include ../makeall.mk
 
#Linux
LIBS = ${W3NCO_LIB4} ${BUFR_LIB4}

#Should be no changes below this point---------------------------------

seaice_ssmisubufr : 
	$(FC) $(FOPTS) ssmisu_decode.f
	$(CC) $(COPTS) ssmisu_binary.c -I $(MMAB_INC) 
	$(FLD) $(FLDFLAGS) ssmisu_decode.o ssmisu_binary.o $(LIBS) -o seaice_ssmisubufr
