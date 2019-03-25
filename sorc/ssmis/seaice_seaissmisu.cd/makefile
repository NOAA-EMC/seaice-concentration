#Robert Grumbine 27 May 2016

include ../makeall.mk

#Linux
LIBS=$(MMAB_LIB) -lombc_4 -lombf_4 -lm
 
#Should be no changes below this point---------------------------------
seaice_seaissmis : ssmis.main.o
	$(CPPLD) -DTEAM1 $< $(LIBS) $(CPPLDFLAGS) -o seaice_seaissmis

ssmis.main.o : ssmis.main.C
	$(CPP) $(CPPOPTS) -DTEAM1 $< 
