#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
  
include ../makeall.mk
 
#Linux
LIBS = ${W3EMC_LIB4} ${BUFR_LIB4}

#Should be no changes below this point---------------------------------

seaice_ssmisubufr : ssmisu_decode.f90 ssmisu_binary.c 
	$(FC) $(FOPTS) ssmisu_decode.f90
	$(CC) $(COPTS) ssmisu_binary.c -I $(MMAB_INC) 
	$(FLD) $(FLDFLAGS) ssmisu_decode.o ssmisu_binary.o $(LIBS) -o $(@)
