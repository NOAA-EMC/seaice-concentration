#Robert Grumbine 27 May 2016

include ../makeall.mk

#Should be no changes below this point---------------------------------
seaice_blend : seaice_blend.o
	$(CPPLD) $(CPPLDFLAGS) $< $(MMAB_LIBF4) -o $(@)
