#Robert Grumbine 27 May 2016

include ../makeall.mk

LIBS = ${W3EMC_LIBd} ${BACIO_LIB4}

FFLAGS =	-O3 -convert big_endian -assume byterecl -real-size 64 -std90 -pg
CFLAGS = 	-O2 -DLINUX -std=c89 -pg

OBJS = seaice_iceamsr2.o

seaice_iceamsr2 :  $(OBJS)
	$(CPPLD) $(CPPLDFLAGS) $(OBJS) -o $(@) $(MMAB_LIBF4) 

