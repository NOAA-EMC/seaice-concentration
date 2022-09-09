#Robert Grumbine 27 May 2016
SHELL=/bin/sh
        
include ../makeall.mk
#work -- from makeall.mk or makeall.sh as a rule
#
#Home:
#NCEPLIB=-L /Users/rmg3/usrlocal/nceplibs/lib
#LIBS=-l ombf_4 -l bufr_4_DA -l w3nco_4 
#Ncep
NCEPLIB=$(BUFR_LIB4) $(W3EMC_LIB4) $(W3NCO_LIB4)

#Should be no changes below this point---------------------------------

all : ssmi_tol2 ssmisu_tol2 

ssmisu_tol2 : ssmisu_algorithm.o filter.o bufr.o skel.o ssmisu_decode.o
	$(FC) ssmisu_decode.o filter.o skel.o bufr.o ssmisu_algorithm.o  -L $(NETCDF_LIBRARIES) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

ssmi_tol2 : ssmi_algorithm.o ssmi_filter.o ssmi_bufr.o ssmi.bufr.o ssmi_binary.o ssmi_skel.o 
	$(FC) ssmi.bufr.o ssmi_filter.o ssmi_skel.o ssmi_bufr.o ssmi_algorithm.o ssmi_binary.o  -L $(NETCDF_LIBRARIES) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

amsr_tol2 : amsr_algorithm.o filter.o bufr.o skel.o amsr_decode.o
	$(FC) amsr_decode.o filter.o skel.o bufr.o amsr_algorithm.o  -L $(NETCDF_LIBRARIES) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)

# ---------------------------------------------------------------------
clean :
	rm *.o

distclean : clean
	rm ssmi_tol2 ssmis_tol2 bayes demo
# ---------------------------------------------------------------------
bayes : bayes.o
#	g++ $<  -L $(NETCDF_LIBRARIES) $(NCEPLIB) $(LIBS) -o $(@)
	$(CPP) $< -L  $(NETCDF_LIBRARIES) $(NCEPLIB) $(LIBS) -o $(@)

demo : demo.o
#	g++ $<  -L $(NETCDF_LIBRARIES) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)
	$(CPP) $< -L  $(NETCDF_LIBRARIES) -lnetcdf  $(NCEPLIB) $(LIBS) -o $(@)
