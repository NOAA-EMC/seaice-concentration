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

all : ssmi_tol3 ssmisu_tol3 

ssmi_tol3 : ssmi_tol3.o
	$(CPPLD) $(@).o $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB) -L ~/rgdev/mmablib/ $(LIBS) $(CPPLDFLAGS) -o $(@)

ssmisu_tol3 : ssmisu_tol3.o
	$(CPPLD) $(@).o $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB) -L ~/rgdev/mmablib/ $(LIBS) $(CPPLDFLAGS) -o $(@)

# ---------------------------------------------------------------------
clean :
	rm *.o

distclean : clean
	rm ssmi_tol3 ssmisu_tol3
# ---------------------------------------------------------------------
