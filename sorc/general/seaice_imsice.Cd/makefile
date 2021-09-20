#Robert Grumbine 27 May 2016

include ../makeall.mk

#==========================================================
all : imsice imsice_gl 

imsice.o : imsice.C
	$(CPP) $(CPPOPTS) -DGRIDTYPE=global_12th $<
imsice : imsice.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4) 

imsice_gl.o : imsice_gl.C
	$(CPP) $(CPPOPTS) -DGRIDTYPE=great_lakes_wave $< -o $(@)
imsice_gl : imsice_gl.o
	$(CPPLD) $(CPPLDFLAGS) $< -o $@ $(MMAB_LIBF4)
