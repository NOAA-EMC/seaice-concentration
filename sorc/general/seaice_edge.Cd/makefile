#Robert Grumbine 27 May 2016

include ../makeall.mk

#-----------------------------------------------------

all: seaice_edge

seaice_edge : seaice_edge.o
	$(CPPLD) $(CPPLDFLAGS) seaice_edge.o -o seaice_edge $(MMAB_LIBF4) 

seaice_edge.o : seaice_edge.C 
	$(CPP) $(CPPOPTS) seaice_edge.C  -DANALYSIS=global_12th -DMODEL=global_12th 
