#Makefile for generating the executables for the sea ice concentration
#  product.
#Robert Grumbine 26 June 1997
 
include ../makeall.mk

LIBS=-lm

#Should be no changes below this point---------------------------------

seaice_icegrid : fill.o 
	$(CPPLD) $(CPPLDFLAGS) $< $(MMAB_LIBF4) $(LIBS) -o $(@)

fill.o : fill.C 
	$(CPP) $(CPPOPTS) -DGRIDTYPE=global_12th $< 
	#debug: $(CPP) $(CPPOPTS) -DGRIDTYPE=global_12th fill.C -DVERBOSE -DVERBOSE2 
