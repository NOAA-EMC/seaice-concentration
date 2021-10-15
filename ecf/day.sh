#!/bin/bash --login

export tag=${tag:-`date +"%Y%m%d"`}
tag=20210824
tagm=`expr $tag - 1`
export tagm=`/u/Robert.Grumbine/bin/dtgfix3 $tagm`
export end=$tag
echo initial tag date = $tag

#-----------------------------------------------------------------------------
set -e
echo zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz loading modules zzzzzzzzzzzzzzzzzzzzzzz
#. /usrx/local/Modules/3.2.10/init/bash
module purge
module load envvar/1.0
module load imagemagick/7.0.8-7
module load prod_envir/2.0.5
module load prod_util/2.0.9
#Can be loaded directly -- required for additional modules
module load intel/19.1.3.304
module load craype/2.7.8
module load PrgEnv-intel/8.1.0
#Additional:
module load wgrib2/2.0.8
module load w3nco/2.4.1

module load jasper/2.0.25
module load libpng/1.6.37
module load zlib/1.2.11
module load libjpeg/9c
module load bacio/2.4.1
module load bufr/11.5.0
module load g2/3.4.5
module load netcdf/4.7.4

#module load grib_util/1.1.1
#module load bufr_dumplist/2.3.0
module load bufr_dump/2.0.0
#module load dumpjb/5.1.0
#module load lsf/10.1

module list
echo zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz done loading modules zzzzzzzzzzzzzzzzzz

set -xe

# Bring the various environment-sensitive definitions out of J jobs and to here:
#NCO refers to these as 'job card' variables

export HOMEbase=/u/Robert.Grumbine/rgdev
export seaice_analysis_ver=v4.4.0
export HOMEseaice_analysis=$HOMEbase/seaice_analysis.${seaice_analysis_ver}

cd $HOMEseaice_analysis/ecf/
. ./jobcards
echo $jlogfile $DATA $cyc $cycle
echo date pdy= $PDY ncepdate = $ncepdate


#is this needed any more?
#echo date before obsproc: $PDY
#if [ -z $obsproc_dump_ver ] ; then
#  echo null obsproc_dump_ver
#  export obsproc_dump_ver=v4.0.0
#  export obsproc_shared_bufr_dumplist_ver=v1.4.0
#fi
#echo tag = $tag date after obsproc: $PDY

#--------------------------------------------------------------------------------------
#The actual running of stuff

while [ $tag -le $end ]
do
  export PDY=$tag
  export PDYm1=$tagm
  echo dates: tag= $tag PDY = $PDY
#for canned cactus test:
  export COMIN=/lfs/h1/ops/canned/com/omb/v4.4/sice.$tagm
  export DCOMROOT=/lfs/h1/ops/canned/dcom
  export DCOM=/lfs/h1/ops/canned/dcom/20210824

  export job=seaice_filter
  export DATA=$DATAROOT/${job}.${pid}
  #script handles make: mkdir $DATA
  time ./sms.filter.fake > /u/Robert.Grumbine/noscrub/com/sms.filter.$tag

  export job=seaice_analysis
  export DATA=$DATAROOT/${job}.${pid}
  #script handles make: mkdir $DATA
#Required for dumpjb to run:
  export TMPDIR=$DATA
  env > env_pre_analysis
  time ./sms.fake > /u/Robert.Grumbine/noscrub/com/sms.$tag

#  module load gempak
#  time ../jobs/JICE_GEMPAK > gempak.$tag

  tagm=$tag
  tag=`expr $tag + 1`
  tag=`/u/Robert.Grumbine/bin/dtgfix3 $tag`

done
