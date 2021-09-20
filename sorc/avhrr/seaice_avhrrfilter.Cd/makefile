#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#Should be no changes below this point---------------------------------

seaice_avhrrfilter : gacfilt.o
	$(CPPLD) $(MMAB_LIBF4) $< $(CPPLDFLAGS) -o seaice_avhrrfilter

gacfilt.o : gacfilt.C
	$(CPP) $(CPPOPTS) gacfilt.C 

