#Robert Grumbine 27 May 2016
SHELL=/bin/sh
        
include ../makeall.mk
#work
#module load ips impi
#module load NetCDF
#module load bufr w3nco w3emc
#
NCEPLIB=$(BUFR_LIB4) $(W3EMC_LIB4) $(W3NCO_LIB4)

#Should be no changes below this point---------------------------------

all : ssmis_tol2 bayes demo 

bayes : bayes.o
	g++ $<  $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

demo : demo.o
	g++ $<  $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

ssmis_tol2 : algorithm.o filter.o bufr.o skel.o ssmisu_decode.o
	$(FC) ssmisu_decode.o filter.o skel.o bufr.o algorithm.o  $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

# ---------------------------------------------------------------------
clean :
	rm *.o

distclean : clean
	rm ssmis_tol2 bayes demo
