#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#--- Should be no changes below here -------------------------------------------

all : seaice_northxpm seaice_southxpm seaice_north12xpm seaice_north6xpm seaice_south12xpm seaice_south6xpm seaice_globalxpm seaice_global5minxpm

float : seaice_fnorthxpm seaice_fsouthxpm seaice_fnorth12xpm seaice_fsouth12xpm seaice_fglobalxpm seaice_fglobal5minxpm

seaice_northxpm : seaice_xpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=northgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_southxpm : seaice_xpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=southgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_north12xpm : seaice_xpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=northhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_north6xpm : seaice_xpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=northhigh2
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_south12xpm : seaice_xpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=southhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_south6xpm : seaice_xpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=southhigh2
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_global5minxpm : seaice_xpm.C 
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=global_12th 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

seaice_globalxpm : seaice_xpm.C 
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=global_ice 
	$(CPPLD) $(MMAB_LIBF4) seaice_xpm.o $(CPPLDFLAGS) -o $(@)

#for floating point input grids:
seaice_fnorthxpm : seaice_fxpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=northgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)

seaice_fsouthxpm : seaice_fxpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=southgrid 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=southgrid $(MMAB_LIBF4) -o $(@)

seaice_fnorth12xpm : seaice_fxpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=northhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=northhigh $(MMAB_LIBF4) -o $(@)

seaice_fsouth12xpm : seaice_fxpm.C
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=southhigh 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=southhigh $(MMAB_LIBF4) -o $(@)

seaice_fglobal5minxpm : seaice_fxpm.C 
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=global_12th 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=global_12th $(MMAB_LIBF4) -o $(@)

seaice_fglobalxpm : seaice_fxpm.C 
	$(CPP) $(CPPOPTS) $< -DGRIDTYPE=global_ice 
	$(CPPLD) $(MMAB_LIBF4) seaice_fxpm.o $(CPPLDFLAGS) -o $(@)
	$(CPPLD) $(CPPOPTS) $(CPPLDFLAGS) seaice_fxpm.C -DGRIDTYPE=global_ice $(MMAB_LIBF4) -o $(@)
