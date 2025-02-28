#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#--------------- Should need no changes below here ----------------

all : seaice_regions.northhigh seaice_regions.southhigh seaice_regions.north seaice_regions.south

seaice_regions.northhigh : regions.C
	 $(CPP) $(CPPOPTS) $< -DGRIDTYPE=northhigh -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)

seaice_regions.southhigh : regions.C
	 $(CPP) $(CPPOPTS) $< -DGRIDTYPE=southhigh -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)

seaice_regions.north : regions.C
	 $(CPP) $(CPPOPTS) $< -DGRIDTYPE=northgrid -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)

seaice_regions.south : regions.C
	 $(CPP) $(CPPOPTS) $< -DGRIDTYPE=southgrid -o $(@).o
	 $(CPPLD) $(MMAB_LIBF4) $(@).o $(CPPLDFLAGS) -o $(@)
