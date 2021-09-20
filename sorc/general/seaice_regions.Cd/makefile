#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#--------------- Should need no changes below here ----------------

all : seaice_regions.northhigh seaice_regions.southhigh seaice_regions.north seaice_regions.south

seaice_regions.northhigh : grid8b.C
	 $(CPP) $(CPPOPTS) grid8b.C -DGRIDTYPE=northhigh -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)

seaice_regions.southhigh : grid8b.C
	 $(CPP) $(CPPOPTS) grid8b.C -DGRIDTYPE=southhigh -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)

seaice_regions.north : grid8b.C
	 $(CPP) $(CPPOPTS) grid8b.C -DGRIDTYPE=northgrid -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)

seaice_regions.south : grid8b.C
	 $(CPP) $(CPPOPTS) grid8b.C -DGRIDTYPE=southgrid -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)
