#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#--------------------------------------------------------------------------------

all : seaice_north_reduce seaice_south_reduce seaice_global_reduce seaice_north2_reduce seaice_south2_reduce

seaice_north2_reduce : reduce.C
	$(CPP) $(CPPOPTS) $< -DGRIDLOW=northhigh -DGRIDHIGH=northhigh2 -o $(@).o
	$(CPPLD) $(@).o $(CPPLDFLAGS) $(MMAB_LIBF4) -o $(@) 

seaice_south2_reduce : reduce.C
	$(CPP) $(CPPOPTS) $< -DGRIDLOW=southhigh -DGRIDHIGH=southhigh2 -o $(@).o
	$(CPPLD) $(@).o $(CPPLDFLAGS) $(MMAB_LIBF4) -o $(@) 

seaice_north_reduce : reduce.C
	$(CPP) $(CPPOPTS) $< -DGRIDLOW=northgrid -DGRIDHIGH=northhigh -o $(@).o
	$(CPPLD) $(@).o $(CPPLDFLAGS) $(MMAB_LIBF4) -o $(@) 

seaice_south_reduce : reduce.C
	$(CPP) $(CPPOPTS) $< -DGRIDLOW=southgrid -DGRIDHIGH=southhigh -o $(@).o
	$(CPPLD) $(@).o $(CPPLDFLAGS) $(MMAB_LIBF4) -o $(@) 

seaice_global_reduce : reduce.C
	$(CPP) $(CPPOPTS) $< -DGRIDLOW=global_ice -DGRIDHIGH=global_12th -o $(@).o 
	$(CPPLD) $(@).o $(CPPLDFLAGS) $(MMAB_LIBF4) -o $(@) 
