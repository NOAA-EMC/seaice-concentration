#Robert Grumbine 27 May 2016

include ../makeall.mk

#------------------------------------------------------------
all : noice noice_nc 

noice : noice.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4)

noice.o : noice.C
	$(CPP) $(CPPOPTS) -DGRIDTYPE=global_12th $<

noice_nc : noice_nc.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4) $(NETCDF_LDFLAGS_C) -lnetcdf

noice_nc.o : noice_nc.C
	$(CPP) $(CPPOPTS) -DGRIDTYPE=global_12th $<



