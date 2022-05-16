#!/bin/sh
#Robert Grumbine
#22 December 2016

set -x

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
  set +x
  module purge
  module load envvar/1.0
  module use `pwd`/modulefiles
  module load seaice_analysis/4.5.0
  if [ $? -ne 0 ] ; then
    echo some problem trying to load seaice_analysis/4.5.0
    echo manually loading
    #NCO Compilation modules
    module load PrgEnv-intel/8.2.0
    module load intel/19.1.3.304
    module avail
    #NCO build libraries for grib, bufr, ...
    module load w3nco/2.4.1
    module load bacio/2.4.1
    module load bufr/11.6.0
    module load g2/3.4.5
    module load jasper/2.0.25
    module load libpng/1.6.37
    #module load zlib/1.2.11
    module load netcdf/4.7.4
  fi
  set -x
  module list
  export BASE=/u/Robert.Grumbine/rgdev/mmablib
  export MMAB_BASE=/u/Robert.Grumbine/rgdev/mmablib
  export VER=""
  env
#If being built against new mmablib by developer:
#  export MMAB_INC=$BASE/include/
#  export MMAB_SRC=${BASE}/sorc/
#  export MMAB_LIB="-L ${BASE}/"
#  export MMAB_VER=""
#  export VER=$MMAB_VER
#  export dlib=${BASE}
#  export MMAB_INC=$dlib/include
#  export MMAB_OMBC_LIB4=$dlib/libombc_4.a
#  export MMAB_OMBF_LIB4=$dlib/libombf_4.a

fi
export mmablib_ver=${MMAB_VER:-v3.5.0}

#set -xe
set -x

. ../versions/seaice_analysis.ver

#for d in general amsr2 ssmi ssmis avhrr l1b_to_l2 l2_to_l3
for d in l1b_to_l2 amsr2 ssmi ssmis avhrr  
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

#clean up
rm */makeall.mk
