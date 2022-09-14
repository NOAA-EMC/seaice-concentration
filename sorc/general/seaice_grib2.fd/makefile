SHELL=/bin/sh
#Robert Grumbine 27 May 2016

include ../makeall.mk
#
SRCS=	seaice_grib2.f
OBJS=	seaice_grib2.o

#LIBS = ${BACIO_LIB4} ${W3NCO_LIB4} ${G2_LIB4} ${JASPER_LIB} ${PNG_LIB} ${Z_LIB}
LIBS = ${BACIO_LIB4} ${W3NCO_LIB4} ${G2_LIB4} ${JASPER_LIB} ${LIBPNG_LIB} ${ZLIB_LIB}

CMD =	seaice_grib2
PROFLIB =	-lprof

# Lines from here on down should not need to be changed.  They are the
# actual rules which make uses to build the executable
#
all:		$(CMD)

$(CMD):		$(OBJS)
	$(FLD) $(FLDFLAGS) -o $(@) $(OBJS) $(LIBS)

# Make the profiled version of the command and call it a.out.prof
#
$(CMD).prof:	$(OBJS)
	$(FLD) $(FLDFLAGS) -o $(@) $(OBJS) $(PROFLIB) $(LIBS)

clean:
	-rm -f $(OBJS)

clobber:	clean
	-rm -f $(CMD) $(CMD).prof

void:	clobber
	-rm -f $(SRCS) makefile
