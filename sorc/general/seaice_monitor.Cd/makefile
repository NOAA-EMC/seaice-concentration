#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#-------------------------------------------------------------------------------

seaice_monitor_c12th : seaice_monitor.C 
	$(CPP) $(CPPOPTS) seaice_monitor.C -DGRIDTYPE=global_12th -DDATTYPE="unsigned char" -o $(@).o
	$(CPPLD) $(MMAB_LIB) $(@).o $(CPPLDFLAGS) -o $(@)
