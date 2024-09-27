#Robert Grumbine 27 May 2016

include ../makeall.mk

#------------------------------------------------------------
all : noice 

noice.o : noice.C
	$(CPP) $(CPPOPTS)  -DGRIDTYPE=global_12th  $<
noice : noice.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4)
