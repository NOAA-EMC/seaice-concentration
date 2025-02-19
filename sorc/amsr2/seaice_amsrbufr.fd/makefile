#Robert Grumbine 27 May 2016

include ../makeall.mk

LIBS = ${W3EMC_LIBd} ${BUFR_LIB4} ${BACIO_LIB4}

#FFLAGS =	-O3 -convert big_endian -assume byterecl -real-size 64 -std90
CFLAGS = 	-O2 -DLINUX -std=c89 -pg
OBJS=		amsr2.o amsr2out.o
CMD =		seaice_amsrbufr

all:		$(CMD)

$(CMD):		$(OBJS)
	$(FC) $(FLDFLAGS) -o $(@) $(OBJS) $(LIBS)

