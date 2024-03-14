#Robert Grumbine 27 May 2016

include ../makeall.mk

#LIBS=   -L/nwprod/lib -lw3nco_v2.0.3_d -lbufr_v10.2.3_d_64 -lbacio_v2.0.1_4
LIBS = ${W3EMC_LIBd} ${BUFR_LIBd} ${BACIO_LIB4}
#FLDFLAGS =	-pg	
#FFLAGS =	-O3 -convert big_endian -assume byterecl -real-size 64 -std90
CFLAGS = 	-O2 -DLINUX -std=c89 -pg
OBJS=		amsr2.o amsr2out.o
CMD =		seaice_amsrbufr

all:		$(CMD)

$(CMD):		$(OBJS)
	$(FC) $(FLDFLAGS) -o $(@) $(OBJS) $(LIBS)

