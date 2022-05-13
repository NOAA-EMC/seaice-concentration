#MMABDIR=/Volumes/ncep/mmablib.git/mmablib
#MMABDIR=~/para/mmablib/mmablib/
MMABDIR=$(MMAB_BASE)
PLATFORM=F15

all : ssmil3tol2_$(PLATFORM) reread_$(PLATFORM)

ssmil3tol2_$(PLATFORM) : ssmil3tol2.C
	$(CPP) -Wall -O2 -DLINUX -D$(PLATFORM) $< -o $(@)  \
               -I $(MMABDIR)/include* $(MMAB_LIB)/libombf*.a

reread_$(PLATFORM) : reread.C
	$(CPP) -Wall -O2 -DLINUX -D$(PLATFORM) $< -o $(@) \
               -I $(MMABDIR)/include* $(MMAB_LIB)/libombf*.a
