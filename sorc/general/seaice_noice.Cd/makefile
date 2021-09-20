#Robert Grumbine 27 May 2016

include ../makeall.mk

#------------------------------------------------------------
all : noice noice_gl 

noice.o : noice.C
	$(CPP) $(CPPOPTS)  -DGRIDTYPE=global_12th  $<
noice : noice.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4)

noice_gl.o : noice_gl.C
	$(CPP) $(CPPOPTS)  -DGRIDTYPE=great_lakes_wave  $< -o $(@)
noice_gl : noice_gl.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4)
