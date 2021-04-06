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
  module use `pwd`/modulefiles
  module load seaice_analysis/4.3.0
  if [ $? -ne 0 ] ; then
    echo some problem trying to load seaice_analysis/4.3.0
    #NCO Compilation modules
    module load EnvVars/1.0.3 ips/19.0.5.281  impi/19.0.5
    #NCO build libraries for grib, bufr, ...
    module load w3nco/2.2.0 w3emc/2.4.0
    module load bacio/2.0.3
    module load libpng/1.2.59
    module load jasper/1.900.29
    module load g2/3.2.0
    module load zlib/1.2.11
    module load bufr/11.3.1

  fi
  module list
#If being built against new mmablib by developer:
  export BASE=/u/Robert.Grumbine/rgdev/mmablib
  export VER=""
  export MMAB_INC=$BASE/include/
  export MMAB_SRC=${BASE}/sorc/
  export MMAB_LIB="-L ${BASE}/"
  export MMAB_VER=""
  export VER=$MMAB_VER
  export dlib=${BASE}
  export MMAB_INC=$dlib/include
  export MMAB_OMBC_LIB4=$dlib/libombc_4.a
  export MMAB_OMBF_LIB4=$dlib/libombf_4.a

fi
export mmablib_ver=${MMAB_VER:-v3.5.0}

set -xe

. ../versions/seaice_analysis.ver

for d in general amsr2 ssmi ssmis avhrr l1b_to_l2 l2_to_l3
do
  cp makeall.mk $d
  cd $d
  ./makeall.sh
  cd ..
done

if [ ! -d ../exec ] ; then
  mkdir ../exec
fi
./toexec cp
