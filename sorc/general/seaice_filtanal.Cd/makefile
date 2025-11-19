#Robert Grumbine 27 May 2016

include ../makeall.mk

#-----------------------------------------------------
all: seaice_filtanal 

seaice_filtanal : filt3.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $(@) $(MMAB_LIBF4) 

