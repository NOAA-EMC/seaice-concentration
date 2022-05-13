#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997

include ../makeall.mk

#Linux
LIBS = $(BUFR_LIBd)

#Should be no changes below this point---------------------------------

seaice_ssmibufr : 
	$(FC) $(FOPTS) ssmi.bufr.f
	$(CC) $(COPTS) binary.c -I $(MMAB_INC)
	$(FLD) ssmi.bufr.o binary.o $(LIBS) -o seaice_ssmibufr
