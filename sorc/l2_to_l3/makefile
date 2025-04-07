#Robert Grumbine 27 May 2016
SHELL=/bin/sh
        
include ../makeall.mk
#work -- from makeall.mk or makeall.sh as a rule
NCEPLIB=$(BUFR_LIB4) $(W3EMC_LIB4) 

#Should be no changes below this point---------------------------------

all : ssmisu_tol3 

ssmisu_tol3 : ssmisu_tol3.o
	$(CPPLD) $(@).o $(NETCDF_LDFLAGS_C) -lnetcdf  $(NCEPLIB)  $(LIBS) $(CPPLDFLAGS) -o $(@)

# ---------------------------------------------------------------------
clean :
	rm *.o

distclean : clean
	rm ssmi_tol3 ssmisu_tol3
# ---------------------------------------------------------------------
