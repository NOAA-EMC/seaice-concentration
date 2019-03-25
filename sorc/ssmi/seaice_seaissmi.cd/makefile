#Robert Grumbine 27 May 2016

include ../makeall.mk

LIBS=$(MMAB_LIB) -lombc_4 -lombf_4 -lm

#Should be no changes below this point---------------------------------

seaice_seaissmi : ssmi.main.o
	$(CPPLD) $(CPPLDFLAGS) ssmi.main.o $(LIBS) -o seaice_seaissmi

ssmi.main.o : ssmi.main.C
	$(CPP) $(CPPOPTS) ssmi.main.C 
