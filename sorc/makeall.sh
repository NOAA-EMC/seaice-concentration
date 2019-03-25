#!/bin/sh
#Robert Grumbine
#22 December 2016


module list > /dev/null 2> /dev/null
if [ $? -ne 0 ] ; then
#On a system without the module software
  if [ `uname` == 'Darwin' ] ; then
    export BASE=/Users/rmg3/usrlocal/mmab
    export MMAB_VER=v3.5.0
  else
    export BASE=/usr1/rmg3
    export MMAB_VER=v3.5.0
  fi
  export VER=$MMAB_VER
  export MMAB_INC=$BASE/$VER/include/
  export MMAB_LIB="-L ${BASE}/mmablib/$VER/"
  export MMAB_SRC=${BASE}/${VER}/sorc/
  #end building on non-module system
else
#on a system with module software, such as wcoss
  module purge
  module --ignore-cache load "./seaice_analysis.modulefile"
  if [ $? -ne 0 ] ; then
    echo some problem trying to load ./seaice_analysis.modulefile
    module load EnvVars/1.0.2 ips/18.0.1.163  impi/18.0.1
    module load w3nco/2.0.6 w3emc/2.3.0
    module load bufr/11.2.0 bacio/2.0.2
    module load libpng/1.2.44
    module load zlib/1.2.11
    module load jasper/1.900.1
    module load g2/3.1.0

  fi
  module list
#If being built against new mmablib by developer:
  export BASE=~/para/mmablib/
  export MMAB_VER=v3.5.0
  export VER=$MMAB_VER
  export MMAB_INC=${BASE}/$VER/include/
  export MMAB_SRC=${BASE}/$VER/sorc/
  export MMAB_LIB='-L ${BASE}/${VER}/'
#from nco modulefile in /nwprod2/lib/modulefiles/mmab
  export dlib=${BASE}/${VER}
  export MMAB_INC=$dlib/include
  export MMAB_OMBC_LIB4=$dlib/libombc_4.a
  export MMAB_OMBF_LIB4=$dlib/libombf_4.a

fi
export mmablib_ver=${MMAB_VER:-v3.5.0}

set -xe

. ../versions/seaice_analysis.ver

for d in general amsr2 ssmi ssmis avhrr 
do
  cp makeall.mk $d
  cd $d
  ./makeall.sh
  cd ..
done
./toexec cp
