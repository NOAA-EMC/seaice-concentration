#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#--- Should be no changes below here -------------------------------------------
#Note that the .x is important -- the programs will be reading data files
#  of the same name, differing only in .x for the executable.

all : seaice_posteriori_30min.x seaice_posteriori_5min.x

seaice_posteriori_30min.x :
	$(CPP) $(CPPOPTS) seaice_posteriori.C -DGLOBAL=global_ice -DNORTH=northgrid -DSOUTH=southgrid -o seaice_posteriori_30min.o
	$(CPPLD) seaice_posteriori_30min.o $(MMAB_LIBF4) $(CPPLDFLAGS) -o $(@)

seaice_posteriori_5min.x :
	$(CPPLD) $(CPPOPTS) seaice_posteriori.C -DGLOBAL=global_12th -DNORTH=northhigh -DSOUTH=southhigh -o seaice_posteriori_5min.o
	$(CPPLD) seaice_posteriori_5min.o $(MMAB_LIBF4) $(CPPLDFLAGS) -o $(@)
