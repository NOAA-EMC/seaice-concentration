#MMABDIR=/Volumes/ncep/mmablib.git/mmablib
MMABDIR=~/para/mmablib/mmablib/

PLATFORM=F15

all : ssmil3tol2_$(PLATFORM) reread_$(PLATFORM)

ssmil3tol2_$(PLATFORM) : ssmil3tol2.C
	g++ -Wall -O2 -DLINUX -D$(PLATFORM) $< -o $(@)  \
               -I $(MMABDIR)/include* $(MMABDIR)/libombf*.a

reread_$(PLATFORM) : reread.C
	g++ -Wall -O2 -DLINUX -D$(PLATFORM) $< -o $(@) \
               -I $(MMABDIR)/include* $(MMABDIR)/libombf*.a
