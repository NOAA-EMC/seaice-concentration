#!/bin/sh
#Robert Grumbine
#22 December 2016

set -x

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
  set +x
  module purge
  module use `pwd`/modulefiles
  module load seaice_analysis/4.4.0
  if [ $? -ne 0 ] ; then
    echo some problem trying to load seaice_analysis/4.4.0
  #Phase3:
    #module load EnvVars/1.0.3 ips/19.0.5.281  impi/19.0.5
    #module load mmab/3.5.0
    #module load w3nco/2.2.0 w3emc/2.4.0
    #module load bacio/2.0.3
    #module load libpng/1.2.59
    #module load jasper/1.900.29
    #module load g2/3.2.0
    #module load zlib/1.2.11
    #module load bufr/11.3.1
    #module load NetCDF/4.5.0

    export MMAB_BASE=/u/Robert.Grumbine/rgdev/mmablib/
    export MMAB_INC=$MMAB_BASE/include
    export MMAB_SRC=$MMAB_BASE/sorc
    export MMAB_LIBF4=$MMAB_BASE/libombf_4.a
    module load envvar/1.0
    module load imagemagick/7.0.8-7
    module load prod_envir/2.0.5
    module load prod_util/2.0.8
    module load intel/19.1.3.304
    module load craype/2.7.8
    module load PrgEnv-intel/8.1.0

    module load wgrib2/2.0.8
    module load w3nco/2.4.1
    module load jasper/2.0.25
    module load libpng/1.6.37
    module load zlib/1.2.11
    module load libjpeg/9c
    module load bacio/2.4.1
    module load bufr/11.5.0
    module load g2/3.4.5

    module load hdf5/1.10.6 
    module load netcdf/4.7.4 
    module load cray-mpich/8.1.7 
    module load grib_util/1.2.3 
    module load w3emc/2.7.3

    module list
  fi
  set -x
  module list
  env
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
export mmablib_ver=${MMAB_VER:-v3.5.0}

#set -xe
set -x

#. ../versions/seaice_analysis.ver

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
#rm */makeall.mk
