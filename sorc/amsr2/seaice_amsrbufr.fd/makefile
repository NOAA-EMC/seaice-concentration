#Robert Grumbine 27 May 2016

include ../makeall.mk

LIBS = ${W3NCO_LIBd} ${BUFR_LIBd} ${BACIO_LIB4}
CFLAGS = 	-O2 -DLINUX -std=c89 -pg
OBJS=		amsr2.o amsr2out.o
CMD =		seaice_amsrbufr

all:		$(CMD)

$(CMD):		$(OBJS)
	$(FC) $(FLDFLAGS) -o $(@) $(OBJS) $(LIBS)

