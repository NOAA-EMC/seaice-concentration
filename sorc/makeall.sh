#!/bin/sh
#Robert Grumbine
#22 December 2016

set -x

source ../versions/build.ver
module list > /dev/null 2> /dev/null

if [ $? -ne 0 ] ; then
#On a system without the module software
  if [ `uname` == 'Darwin' ] ; then
    export MMAB_BASE=/Users/rmg3/usrlocal/mmablib/
    export MMAB_VER=v3.5.0
  else
    export MMAB_BASE=/usr1/rmg3
    export MMAB_VER=v3.5.0
  fi
  export VER=$MMAB_VER
  export MMAB_INC=$MMAB_BASE/$VER/include/
  export MMAB_LIB="-L ${MMAB_BASE}/$VER/"
  export MMAB_SRC=${MMAB_BASE}/${VER}/sorc/
  #end building on non-module system
else
#on a system with module software, such as wcoss
#  set +x
  module reset
  module use `pwd`/modulefiles
  module load seaice_analysis/$seaice_analysis_ver
  if [ $? -ne 0 ] ; then
    echo some problem trying to load seaice_analysis/$seaice_analysis_ver
    exit 1
  fi
  set -x
  module list
  env

  # fix MMAB?
  # module load mmab/$MMAB_VER

  export MMAB_BASE=`pwd`/mmablib/${MMAB_VER}
  export MMAB_INC=$MMAB_BASE/include
  export MMAB_SRC=$MMAB_BASE/sorc
  export MMAB_LIBF4=$MMAB_BASE/libombf_4.a
#If being built against new mmablib by developer:
#  export BASE=/u/Robert.Grumbine/rgdev/mmablib
#  export VER=""
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
export mmablib_ver=${MMAB_VER}

#set -xe
set -x

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

#clean up
rm */makeall.mk
