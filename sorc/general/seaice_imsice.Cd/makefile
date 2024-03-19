#Robert Grumbine 27 May 2016

include ../makeall.mk

#==========================================================
all : imsice 

imsice.o : imsice.C
	$(CPP) $(CPPOPTS) -DGRIDTYPE=global_12th $<
imsice : imsice.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4) 

