#Robert Grumbine 11 Aug 2016
#SHELL=/bin/sh
#
include ../makeall.mk

SRCS=	gac_readavhrr.f90 gac_binary.c
OBJS=	gac_readavhrr.o gac_binary.o
# Tunable parameters
#
LIBS =	   $(W3EMC_LIBd) $(BUFR_LIB4)
CMD =	    seaice_avhrrbufr

# Lines from here on down should not need to be changed. =========================== 

all:		$(CMD)

$(CMD):		$(OBJS)
	echo cmd $(CMD) at $(@)
	$(FLD) $(FLDFLAGS) -o $(@) $(OBJS) $(LIBS)

gac_binary.o: gac_binary.c 
	$(CC) $(COPTS) gac_binary.c

clean:
	-rm -f $(OBJS)

clobber:	clean
	-rm -f $(CMD) $(CMD).prof

void:	clobber
	-rm -f $(SRCS) makefile.readamsre
