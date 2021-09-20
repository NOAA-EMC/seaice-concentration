#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#--- Should be no changes below here -------------------------------------------

all : seaice_northxpm seaice_southxpm seaice_north12xpm seaice_south12xpm seaice_globalxpm seaice_global5minxpm

float : seaice_fnorthxpm seaice_fsouthxpm seaice_fnorth12xpm seaice_fsouth12xpm seaice_fglobalxpm seaice_fglobal5minxpm

seaice_northxpm :
	$(CPP) $(CPPOPTS) seaice_xpm.C -DGRIDTYPE=northgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_southxpm :
	$(CPP) $(CPPOPTS) seaice_xpm.C -DGRIDTYPE=southgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_north12xpm :
	$(CPP) $(CPPOPTS) seaice_xpm.C -DGRIDTYPE=northhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_south12xpm :
	$(CPP) $(CPPOPTS) seaice_xpm.C -DGRIDTYPE=southhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_global5minxpm : 
	$(CPP) $(CPPOPTS) seaice_xpm.C -DGRIDTYPE=global_12th 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_globalxpm : 
	$(CPP) $(CPPOPTS) seaice_xpm.C -DGRIDTYPE=global_ice 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

#for floating point input grids:
seaice_fnorthxpm :
	$(CPP) $(CPPOPTS) seaice_fxpm.C -DGRIDTYPE=northgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)

seaice_fsouthxpm :
	$(CPP) $(CPPOPTS) seaice_fxpm.C -DGRIDTYPE=southgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=southgrid $(MMAB_LIBF4) -o $(@)

seaice_fnorth12xpm :
	$(CPP) $(CPPOPTS) seaice_fxpm.C -DGRIDTYPE=northhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=northhigh $(MMAB_LIBF4) -o $(@)

seaice_fsouth12xpm :
	$(CPP) $(CPPOPTS) seaice_fxpm.C -DGRIDTYPE=southhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=southhigh $(MMAB_LIBF4) -o $(@)

seaice_fglobal5minxpm : 
	$(CPP) $(CPPOPTS) seaice_fxpm.C -DGRIDTYPE=global_12th 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=global_12th $(MMAB_LIBF4) -o $(@)

seaice_fglobalxpm : 
	$(CPP) $(CPPOPTS) seaice_fxpm.C -DGRIDTYPE=global_ice 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=global_ice $(MMAB_LIBF4) -o $(@)
