#Robert Grumbine 11 Aug 2016

include ../makeall.mk

#Should be no changes below this point---------------------------------

all : seaice_viirs 

seaice_viirs : viirs.o
	$(CPPLD) $(MMAB_LIBF4) $< $(CPPLDFLAGS) -o $(@)

viirs_blend : blend.o
	$(CPPLD) $(MMAB_LIBF4) $< $(CPPLDFLAGS) -o $(@)
